// translate.hpp
//
// /translate and its siblings, the right-click "Translate to Aurebesh", and the
// Customize controls under every translation.

#pragma once

#include <algorithm>
#include <charconv>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <variant>

#include <dpp/dpp.h>

#include "render_aurebesh.hpp"
#include "replies.hpp"
#include "scripts.hpp"
#include "style.hpp"
#include "theme.hpp"

// ----------------------------------------------------- the translation message

// Discord caps embed descriptions at 4096 characters, and the translation
// embed repeats the text, so the text is capped well below that. The cap also
// keeps the image to a size people can read.
inline constexpr int max_text_length = 1000;

// Whose words a translation shows, and for one made from the right-click menu,
// a link back to the original message.
struct Attribution {
    std::string name;
    std::string avatar;
    std::string link;
};

inline Attribution attribution_of(const dpp::interaction& command) {
    return {display_name(command), avatar_url(command), ""};
}

// Restyling keeps whoever the translation already credits.
inline Attribution attribution_of(const dpp::embed& embed) {
    return {embed.author ? embed.author->name : "", embed.author ? embed.author->icon_url : "", embed.url};
}

inline dpp::embed translation_embed(const Style& style, const std::string& text, const Attribution& by) {
    dpp::embed embed = dpp::embed()
        .set_title(theme::titled(theme::icon::translate, style.script_info().name))
        .set_description(">>> " + text)
        .set_color(style.color_value())
        .set_image("attachment://aurebesh.png");
    if (!by.name.empty()) embed.set_author(by.name, "", by.avatar);
    if (!by.link.empty()) embed.set_url(by.link);
    return embed;
}

// The text a translation was made from, read back out of its embed.
inline std::optional<std::string> translation_text(const dpp::message& message) {
    if (message.embeds.empty()) return std::nullopt;
    std::string_view description = message.embeds.front().description;
    if (!description.starts_with(">>> ")) return std::nullopt;
    description.remove_prefix(4);
    return std::string(description);
}

// ---------------------------------------------------------- Customize controls

// Every translation has a Customize button. It opens menus for the script,
// color, and background, and buttons for the glow, a reset, and closing the
// menus again. Each control's id carries what the bot needs to redraw the
// translation: "style:<action>:<owner's user id>:<style>".
inline constexpr std::string_view style_prefix = "style:";

inline std::string style_id(std::string_view action, dpp::snowflake owner, const Style& style) {
    return std::string(style_prefix) + std::string(action) + ":" + std::to_string(static_cast<std::uint64_t>(owner)) + ":" +
           encode_style(style);
}

struct StyleRequest {
    std::string action;
    dpp::snowflake owner;
    Style style;
};

inline std::optional<StyleRequest> parse_style_id(std::string_view id) {
    if (!id.starts_with(style_prefix)) return std::nullopt;
    id.remove_prefix(style_prefix.size());
    const std::size_t first = id.find(':');
    const std::size_t second = first == std::string_view::npos ? first : id.find(':', first + 1);
    if (second == std::string_view::npos) return std::nullopt;

    const std::string_view owner_text = id.substr(first + 1, second - first - 1);
    std::uint64_t owner = 0;
    const auto [end, error] = std::from_chars(owner_text.data(), owner_text.data() + owner_text.size(), owner);
    if (error != std::errc() || end != owner_text.data() + owner_text.size()) return std::nullopt;

    const std::optional<Style> style = decode_style(id.substr(second + 1));
    if (!style) return std::nullopt;
    return StyleRequest{std::string(id.substr(0, first)), dpp::snowflake(owner), *style};
}

inline void add_style_controls(dpp::message& message, dpp::snowflake owner, const Style& style, bool open) {
    if (!open) {
        message.add_component(dpp::component().add_component(dpp::component()
            .set_label("Customize").set_emoji(std::string(theme::icon::customize)).set_style(dpp::cos_secondary).set_id(style_id("open", owner, style))));
        return;
    }

    const auto menu = [&](std::string_view action, std::string_view placeholder) {
        return dpp::component().set_type(dpp::cot_selectmenu).set_placeholder(placeholder).set_id(style_id(action, owner, style));
    };

    dpp::component script_menu = menu("script", "Script");
    for (std::size_t i = 0; i < scripts.size(); ++i) {
        if (scripts[i].group.empty()) continue;
        script_menu.add_select_option(dpp::select_option(std::string(scripts[i].name), std::to_string(i), std::string(scripts[i].group))
            .set_default(i == style.script));
    }

    dpp::component color_menu = menu("color", "Color");
    for (std::size_t i = 0; i < colors.size(); ++i) {
        color_menu.add_select_option(dpp::select_option(std::string(colors[i].name), std::to_string(i), i == 0 ? "Each script's own color" : "")
            .set_emoji(std::string(colors[i].emoji)).set_default(i == style.color));
    }

    dpp::component background_menu = menu("background", "Background");
    for (std::size_t i = 0; i < backgrounds.size(); ++i) {
        background_menu.add_select_option(dpp::select_option(std::string(backgrounds[i].name), std::to_string(i))
            .set_emoji(std::string(backgrounds[i].emoji)).set_default(i == style.background));
    }

    message.add_component(dpp::component().add_component(script_menu));
    message.add_component(dpp::component().add_component(color_menu));
    message.add_component(dpp::component().add_component(background_menu));
    message.add_component(dpp::component()
        .add_component(dpp::component().set_label(style.glow ? "Glow: On" : "Glow: Off").set_emoji("💡")
            .set_style(style.glow ? dpp::cos_primary : dpp::cos_secondary).set_id(style_id("glow", owner, style)))
        .add_component(dpp::component().set_label("Reset").set_emoji("🔄").set_style(dpp::cos_secondary).set_id(style_id("reset", owner, style)))
        .add_component(dpp::component().set_label("Done").set_emoji("✅").set_style(dpp::cos_success).set_id(style_id("close", owner, style))));
}

// The owner is whoever made the translation; only they can restyle it.
inline dpp::message translation_message(const Style& style, const std::string& text, const std::string& png, const Attribution& by,
                                        dpp::snowflake owner, bool open) {
    dpp::message message;
    message.add_embed(translation_embed(style, text, by)).add_file("aurebesh.png", png, "image/png");
    add_style_controls(message, owner, style, open);
    return message;
}

// --------------------------------------------------------- drawing and sending

// How a drawn translation goes out: as a new message or as a change to the
// message whose control was used, either right away or into a response that
// was acknowledged first.
enum class Send { reply, update, finish_reply, finish_update };

inline void send_translation(const dpp::interaction_create_t& event, const Style& style, const std::string& text, const Attribution& by,
                             bool open, Send how) {
    const std::string png = aurebesh::render_png(text, style.script_info().font, style.look());
    if (png.empty()) {
        constexpr std::string_view nothing_to_draw = "There is nothing to draw. Please enter some letters or numbers.";
        if (how == Send::finish_reply) event.edit_response(theme::titled(theme::icon::error, nothing_to_draw));
        else if (how == Send::finish_update) event.owner->log(dpp::ll_warning, "Restyled translation drew nothing");
        else reply_notice(event, nothing_to_draw);
        return;
    }

    const dpp::message message = translation_message(style, text, png, by, event.command.usr.id, open);
    switch (how) {
        case Send::reply: event.reply(message); break;
        case Send::update: event.reply(dpp::ir_update_message, message); break;
        case Send::finish_reply:
        case Send::finish_update: event.edit_response(message); break;
    }
}

// Text up to this length draws in milliseconds, so its image goes out in the
// first response. Longer text could take a slow server close to Discord's
// 3-second limit, so the bot acknowledges first and edits the image in.
inline constexpr std::size_t quick_reply_length = 250;

inline void translate(const dpp::interaction_create_t& event, const Style& style, const std::string& text, const Attribution& by,
                      bool open, bool update) {
    if (text.size() <= quick_reply_length) {
        send_translation(event, style, text, by, open, update ? Send::update : Send::reply);
        return;
    }

    // Draw only once Discord has the acknowledgement, so the edit never
    // arrives before it.
    const auto acknowledge = update ? dpp::ir_deferred_update_message : dpp::ir_deferred_channel_message_with_source;
    event.reply(acknowledge, dpp::message(), [event, style, text, by, open, update](const dpp::confirmation_callback_t& done) {
        if (done.is_error()) {
            event.owner->log(dpp::ll_error, "Could not acknowledge a translation: " + done.get_error().human_readable);
            return;
        }
        // This runs outside main.cpp's try, so catch here too: otherwise a new
        // reply would stay on "thinking" until Discord gives up on it.
        try {
            send_translation(event, style, text, by, open, update ? Send::finish_update : Send::finish_reply);
        } catch (const std::exception& e) {
            event.owner->log(dpp::ll_error, std::string("Translation failed: ") + e.what());
            if (!update) event.edit_response(theme::titled(theme::icon::error, something_went_wrong));
        }
    });
}

// -------------------------------------------------------------------- handlers

// /translate and its siblings. The color and background options are optional.
inline void translate_command(const dpp::slashcommand_t& event, const Script& script) {
    const auto option = [&event](const std::string& name) {
        const dpp::command_value value = event.get_parameter(name);
        const std::string* text = std::get_if<std::string>(&value);
        return text ? *text : std::string();
    };

    Style style = default_style(script);
    if (const auto color = parse_index(option("color"), colors.size())) style.color = *color;
    if (const auto background = parse_index(option("background"), backgrounds.size())) style.background = *background;
    translate(event, style, option("text"), attribution_of(event.command), false, false);
}

// The Customize button and everything it opens. Returns false if the id is
// not one of these controls.
inline bool style_control(const dpp::interaction_create_t& event, std::string_view custom_id, std::string_view choice) {
    const std::optional<StyleRequest> request = parse_style_id(custom_id);
    if (!request) return false;

    if (event.command.usr.id != request->owner) {
        reply_notice(event, "Only <@" + std::to_string(static_cast<std::uint64_t>(request->owner)) +
                                "> can restyle this translation. Make your own with `/translate`.");
        return true;
    }
    const std::optional<std::string> text = translation_text(event.command.msg);
    if (!text) {
        reply_notice(event, "This translation can't be restyled anymore.");
        return true;
    }

    Style style = request->style;
    bool open = true;
    const std::string& action = request->action;
    if (action == "close") {
        open = false;
    } else if (action == "glow") {
        style.glow = !style.glow;
    } else if (action == "reset") {
        style = Style{style.script};
    } else if (action == "script") {
        const auto script = parse_index(choice, scripts.size());
        if (script && !scripts[*script].group.empty()) style.script = *script;
    } else if (action == "color") {
        if (const auto color = parse_index(choice, colors.size())) style.color = *color;
    } else if (action == "background") {
        if (const auto background = parse_index(choice, backgrounds.size())) style.background = *background;
    } else if (action != "open") {
        return false;
    }

    translate(event, style, *text, attribution_of(event.command.msg.embeds.front()), open, true);
    return true;
}

// ----------------------------------------------------- right-click translation

// Chat markup the fonts cannot draw. A mention of a person becomes @name, of a
// role or channel @role or #channel, of a command /name; custom emoji and
// timestamps are left out. Returns nothing for anything else in <...>.
inline std::optional<std::string> plain_tag(std::string_view tag, const dpp::message& message) {
    const auto is_number = [](std::string_view text) {
        return !text.empty() && std::all_of(text.begin(), text.end(), [](char c) { return c >= '0' && c <= '9'; });
    };
    const auto person = [&](std::string_view id) {
        for (const auto& [user, member] : message.mentions) {
            if (std::to_string(static_cast<std::uint64_t>(user.id)) == id) {
                return "@" + (user.global_name.empty() ? user.username : user.global_name);
            }
        }
        return std::string("@someone");
    };

    if (tag.starts_with("@&") && is_number(tag.substr(2))) return "@role";
    if (tag.starts_with("@!") && is_number(tag.substr(2))) return person(tag.substr(2));
    if (tag.starts_with("@") && is_number(tag.substr(1))) return person(tag.substr(1));
    if (tag.starts_with("#") && is_number(tag.substr(1))) return "#channel";
    if (tag.starts_with("t:")) return "";
    const std::size_t last_colon = tag.rfind(':');
    if (last_colon != std::string_view::npos && is_number(tag.substr(last_colon + 1))) {
        if (tag.starts_with(":") || tag.starts_with("a:")) return "";  // <:name:id> custom emoji
        if (tag.starts_with("/")) return std::string(tag.substr(0, tag.find(':')));  // </name:id> command
    }
    return std::nullopt;
}

// A chat message's text as plain words: mentions and emoji resolved as above,
// and the marks of Discord's formatting (bold, code, spoilers, quotes,
// headings) removed so they are not drawn as symbols.
inline std::string plain_text(const dpp::message& message) {
    const std::string& in = message.content;
    std::string out;
    bool line_start = true;
    for (std::size_t i = 0; i < in.size();) {
        if (line_start) {
            for (std::string_view prefix : {">>> ", "> ", "### ", "## ", "# ", "-# "}) {
                if (std::string_view(in).substr(i).starts_with(prefix)) {
                    i += prefix.size();
                    break;
                }
            }
            line_start = false;
            if (i >= in.size()) break;
        }
        if (in[i] == '<') {
            const std::size_t close = in.find('>', i);
            if (close != std::string::npos) {
                if (const std::optional<std::string> plain = plain_tag(std::string_view(in).substr(i + 1, close - i - 1), message)) {
                    out += *plain;
                    i = close + 1;
                    continue;
                }
            }
        }
        bool marker = false;
        for (std::string_view mark : {"**", "__", "~~", "||"}) {
            if (std::string_view(in).substr(i).starts_with(mark)) {
                i += mark.size();
                marker = true;
                break;
            }
        }
        if (marker) continue;
        if (in[i] == '`') {
            ++i;
            continue;
        }
        line_start = in[i] == '\n';
        out += in[i++];
    }
    return out;
}

inline std::string message_link(const dpp::interaction& command, const dpp::message& message) {
    const std::string guild = command.guild_id ? std::to_string(static_cast<std::uint64_t>(command.guild_id)) : "@me";
    return "https://discord.com/channels/" + guild + "/" + std::to_string(static_cast<std::uint64_t>(command.channel_id)) + "/" +
           std::to_string(static_cast<std::uint64_t>(message.id));
}

// The message menu's "Translate to Aurebesh": draws someone's message in Basic
// Aurebesh, credited to them. Whoever used the menu can restyle it.
inline void translate_message_command(const dpp::message_context_menu_t& event) {
    const dpp::message& original = event.get_message();
    const std::string text = plain_text(original);
    if (text.find_first_not_of(" \t\n") == std::string::npos) {
        reply_notice(event, "That message has no text to translate.");
        return;
    }
    if (text.size() > static_cast<std::size_t>(max_text_length)) {
        reply_notice(event, "That message is too long to translate. The limit is " + std::to_string(max_text_length) + " characters.");
        return;
    }

    const dpp::user& author = original.author;
    const Attribution by{display_name(author), author.get_avatar_url(128), message_link(event.command, original)};
    translate(event, Style{}, text, by, false, false);
}
