// holocron.hpp
//
// /holocron, /holocron_jedi, and /holocron_sith: a random quote from a Jedi or
// Sith, with a button that opens another.

#pragma once

#include <cstdint>
#include <random>
#include <span>
#include <string>
#include <string_view>

#include <dpp/dpp.h>

#include "theme.hpp"

inline bool contains(std::string_view text, std::string_view part) {
    return text.find(part) != std::string_view::npos;
}

struct Quote {
    std::string_view author;
    std::string_view text;
};

// ------------------------------------------------------------------------ look

// How a holocron looks depends on who is speaking.
struct HolocronLook {
    std::string_view icon;
    std::string_view name;
    std::uint32_t color;
    std::string_view book;  // emoji before the speaker's name
    std::string image;
};

inline HolocronLook holocron_look(std::string_view author, std::string_view command) {
    if (author == "Bendu") {
        return {theme::icon::bendu, "Bendu's Wisdom", 0xDAA520, "📙",
                "https://static.wikia.nocookie.net/starwars/images/3/34/Bendu.png/revision/latest?cb=20230312180517"};
    }
    if (contains(command, "sith") || contains(author, "Darth") || contains(author, "Count")) {
        return {theme::icon::sith, "Sith Holocron", 0xD0312D, "📕", theme::images + "red.jpg"};
    }
    if (contains(author, "Luke") || contains(author, "Qui") || contains(author, "Yoda")) {
        return {theme::icon::jedi, "Jedi Holocron", 0x228B22, "📗", theme::images + "green.jpg"};
    }
    if (contains(author, "Mace")) {
        return {theme::icon::jedi, "Jedi Holocron", 0x9B4DCA, "📘📕", theme::images + "purple.jpg"};
    }
    if (contains(author, "Temple")) {
        return {theme::icon::jedi, "Jedi Holocron", 0xFFD700, "📒", theme::images + "yellow.jpg"};
    }
    return {theme::icon::jedi, "Jedi Holocron", theme::blue, "📘", theme::images + "blue.jpg"};
}

// ---------------------------------------------------------------------- quotes

// Every quote was checked against film and episode transcripts, or Wookieepedia
// for the game. The quotes live in read-only memory; nothing is copied when
// one is drawn.
inline constexpr Quote jedi_quotes[] = {
    // The prequels and The Clone Wars
    { "Yoda", "Fear is the path to the dark side. Fear leads to anger. Anger leads to hate. Hate leads to suffering." },
    { "Yoda", "Train yourself to let go of everything you fear to lose." },
    { "Yoda", "The dark side clouds everything. Impossible to see, the future is." },
    { "Yoda", "Truly wonderful, the mind of a child is." },
    { "Yoda", "Difficult to see. Always in motion is the future." },
    { "Yoda", "The shroud of the dark side has fallen. Begun, the Clone War has." },
    { "Yoda", "Much to learn, you still have." },
    { "Yoda", "Size matters not. Look at me. Judge me by my size, do you?" },
    { "Yoda", "Adventure. Excitement. A Jedi craves not these things." },
    { "Yoda", "In the end, cowards are those who follow the dark side." },
    { "Yoda", "A challenge lifelong it is, not to bend fear into anger." },

    { "Mace Windu", "This party's over." },
    { "Mace Windu", "The oppression of the Sith will never return. You have lost." },
    { "Mace Windu", "I sense a plot to destroy the Jedi." },
    { "Mace Windu", "If what you've told me is true, you will have gained my trust." },
    { "Mace Windu", "The dark side of the Force surrounds the Chancellor." },
    { "Mace Windu", "We're keepers of the peace, not soldiers." },

    { "Obi-Wan Kenobi", "You were the Chosen One! It was said that you would destroy the Sith, not join them!" },
    { "Obi-Wan Kenobi", "Only a Sith deals in absolutes." },
    { "Obi-Wan Kenobi", "You have allowed this Dark Lord to twist your mind until now... you have become the very thing you swore to destroy." },
    { "Obi-Wan Kenobi", "So uncivilized." },
    { "Obi-Wan Kenobi", "Anakin, my allegiance is to the Republic, to democracy!" },
    { "Obi-Wan Kenobi", "I have failed you, Anakin. I have failed you." },
    { "Obi-Wan Kenobi", "Another happy landing." },
    { "Obi-Wan Kenobi", "Why do I get the feeling you're going to be the death of me?" },
    { "Obi-Wan Kenobi", "Good job." },
    { "Obi-Wan Kenobi", "Hello there." },
    { "Obi-Wan Kenobi", "The Council isn't always right. That's why I'm asking for your help." },

    { "Qui-Gon Jinn", "Feel, don't think. Use your instincts." },
    { "Qui-Gon Jinn", "Your focus determines your reality." },
    { "Qui-Gon Jinn", "The ability to speak does not make you intelligent." },
    { "Qui-Gon Jinn", "There's always a bigger fish." },
    { "Qui-Gon Jinn", "I shall do what I must, Obi-Wan." },
    { "Qui-Gon Jinn", "He is the Chosen One. He will bring balance." },
    { "Qui-Gon Jinn", "Our meeting was not a coincidence. Nothing happens by accident." },

    { "Anakin Skywalker", "Mom, you say the biggest problem in this universe is nobody helps each other." },
    { "Anakin Skywalker", "I'm a Jedi. I know I'm better than this." },
    { "Anakin Skywalker", "Someday I will be the most powerful Jedi ever!" },
    { "Anakin Skywalker", "I want more. And I know I shouldn't." },
    { "Anakin Skywalker", "Sometimes, we must let go of our pride and do what is requested of us." },

    { "Ki-Adi-Mundi", "What about the droid attack on the Wookiees?" },
    { "Ki-Adi-Mundi", "The Sith have been extinct for a millennium." },

    { "Plo Koon", "You have our most humble apologies, little 'Soka. The Council was wrong to accuse you." },

    { "Jedi Temple Guard", "I wondered when you would return." },
    { "Jedi Temple Guard", "Try to fight, and you will fail. The Rebellion will be destroyed. You will die, and your apprentice will become a servant of evil." },
    { "Jedi Temple Guard", "The dark side. It pulls at him, it calls to him. Eventually, he will be consumed by it." },
    { "Jedi Temple Guard", "You will never be strong enough to protect your pupil, and you will perish for your failure." },
    { "Jedi Temple Guard", "By the right of the Council, by the will of the Force, Kanan Jarrus, you may rise." },

    // The original and sequel trilogies
    { "Obi-Wan Kenobi", "Use the Force, Luke." },
    { "Obi-Wan Kenobi", "You can't win, Darth. If you strike me down, I shall become more powerful than you can possibly imagine." },
    { "Obi-Wan Kenobi", "The Force will be with you, always." },
    { "Obi-Wan Kenobi", "This is the weapon of a Jedi Knight. Not as clumsy or random as a blaster; an elegant weapon for a more civilized age." },
    { "Obi-Wan Kenobi", "Mos Eisley spaceport. You will never find a more wretched hive of scum and villainy. We must be cautious." },
    { "Obi-Wan Kenobi", "Your eyes can deceive you. Don't trust them." },
    
    { "Luke Skywalker", "I am a Jedi, like my father before me." },
    { "Luke Skywalker", "I'll never turn to the dark side. You've failed, Your Highness." },
    { "Luke Skywalker", "I feel the good in you. The conflict." },
    { "Luke Skywalker", "I will not fight you, Father." },
    { "Luke Skywalker", "I have accepted the truth that you were once Anakin Skywalker, my father." },
    { "Luke Skywalker", "Search your feelings, Father. You can't do this. I feel the conflict within you." },
    { "Luke Skywalker", "Confronting fear is the destiny of a Jedi." },

    { "Yoda", "Judge me by my size, do you?" },
    { "Yoda", "You must unlearn what you have learned." },
    { "Yoda", "Luminous beings are we, not this crude matter." },
    { "Yoda", "Wars not make one great." },
    { "Yoda", "For my ally is the Force. And a powerful ally it is." },
    { "Yoda", "Do or do not. There is no try." },
    { "Yoda", "Always in motion is the future." },
    { "Yoda", "A Jedi uses the Force for knowledge and defense, never for attack." },
    { "Yoda", "That is why you fail." },
    { "Yoda", "The greatest teacher, failure is." },
    { "Yoda", "The fear of loss is a path to the dark side." },
    { "Yoda", "We are what they grow beyond. That is the true burden of all masters." },

    // Star Wars Rebels
    { "Kanan Jarrus", "I survived one war. I'm not ready for another one." },
    { "Kanan Jarrus", "I have nothing left to fear." },
    { "Kanan Jarrus", "The point is that you're not alone. You're connected to every living thing in the universe." },
    { "Kanan Jarrus", "The Force is trying to tell you something. Listen to it." },
    { "Kanan Jarrus", "I never said you were a Jedi. I said you had the potential to become one." },
    { "Kanan Jarrus", "If all I do is try, that means I don't truly believe I can succeed. So from now on, I will teach you." },
    { "Kanan Jarrus", "If you hang on to your past, if you always try to protect yourself, you'll never be a Jedi." },
    { "Kanan Jarrus", "I lost my way for a long time, but now I have a chance to change things." },
    { "Kanan Jarrus", "I won't let him lose his way, not like I did." },
    { "Kanan Jarrus", "When I was your age, there were around 10,000 Jedi Knights defending the galaxy. Now, we are few." },
    { "Kanan Jarrus", "There's a cost for any action we take now, Ezra." },

    { "Ahsoka Tano", "In my experience, just when you think you understand the Force, you find out how little you actually know." },

    { "Ezra Bridger", "I know what I have to do now, but I'm afraid. Not for me, but for my friends." },
    { "Ezra Bridger", "We can't back down just because we're afraid. That's when we need to stand the tallest." },
    { "Ezra Bridger", "Stand up together. Because that's when we're strongest, as one." },
    { "Ezra Bridger", "Lothal is my home, Kanan. I can't stand seeing it like this." },
    { "Ezra Bridger", "You're wrong. I have a family. I don't need anything from you." },
    { "Ezra Bridger", "The Force isn't a weapon, but you'll never understand that." },
    { "Ezra Bridger", "I want you to know everything I've done and will do began with you." },

    { "Obi-Wan Kenobi", "You're in the wrong place, Ezra Bridger." },

    // Star Wars Jedi: Fallen Order
    { "Cal Kestis", "Master. I will never forget. The loss has become a part of me. I will honor your teaching. And your sacrifice." },
    { "Cal Kestis", "I... I might be one of the last of my kind." },
    { "Cal Kestis", "Every Jedi comes here as a kid. Or, they did. When there were Jedi." },
    { "Cal Kestis", "I feel like I'm surrounded by ghosts." },
    { "Cal Kestis", "There's still so much the Empire hasn't touched..." },
    { "Cal Kestis", "Memories that have haunted me since Bracca... I won't run from them anymore." },
    { "Cal Kestis", "It's no way to live. Not for a Jedi. Or a droid. Maybe Cere was right. Maybe we're done hiding." },
    { "Cal Kestis", "Hey. We're here to free you. You're not alone." },
    { "Cal Kestis", "They broke them, beat them down, and turned them to the dark side." },

    { "Jaro Tapal", "Remember... trust only... in the Force." },
};

inline constexpr Quote sith_quotes[] = {
    // The prequels and The Clone Wars
    { "Darth Sidious", "The dark side of the Force is a pathway to many abilities some consider to be unnatural." },
    { "Darth Sidious", "I am the Senate." },
    { "Darth Sidious", "Your feeble skills are no match for the power of the Dark Side." },
    { "Darth Sidious", "Everything that has transpired has done so according to my design." },
    { "Darth Sidious", "Now you will experience the full power of the dark side." },
    { "Darth Sidious", "Once more, the Sith will rule the galaxy!" },
    { "Darth Sidious", "Do what must be done, Lord Vader. Do not hesitate. Show no mercy." },
    { "Darth Sidious", "Your arrogance blinds you, Master Yoda." },
    { "Darth Sidious", "Wipe them out... all of them." },

    { "Count Dooku", "Twice the pride, double the fall." },
    { "Count Dooku", "I've become more powerful than any Jedi." },
    { "Count Dooku", "Master Kenobi, you disappoint me. Yoda holds you in such high esteem. Surely you can do better." },
    { "Count Dooku", "I sense great fear in you, Skywalker. You have hate. You have anger. But you don't use them." },
    { "Count Dooku", "It is obvious that this contest cannot be decided by our knowledge of the Force... but by our skills with a lightsaber." },
    { "Count Dooku", "What if I told you that the Republic was now under the control of the Dark Lord of the Sith?" },

    { "Darth Maul", "At last we will reveal ourselves to the Jedi. At last we will have revenge." },
    { "Darth Maul", "I was hoping for Kenobi. Why are you here?" },
    { "Darth Maul", "You may have forgotten me... but I will never forget you." },
    { "Darth Maul", "There is no justice, no law, no order, except for the one that will replace it!" },
    { "Darth Maul", "You cannot imagine the depths I would go to to stay alive, fueled by my singular hatred for you." },

    { "Darth Vader", "You underestimate my power!" },
    { "Darth Vader", "If you're not with me, then you're my enemy." },
    { "Darth Vader", "From my point of view, the Jedi are evil!" },
    { "Darth Vader", "You will not take her from me!" },
    { "Darth Vader", "I have brought peace, freedom, justice, and security to my new empire." },

    // The original and sequel trilogies, and Star Wars Rebels
    { "Darth Vader", "You don't know the power of the dark side!" },
    { "Darth Vader", "I find your lack of faith disturbing." },
    { "Darth Vader", "The Force is strong with this one." },
    { "Darth Vader", "It is your destiny. Join me, and together we can rule the galaxy." },
    { "Darth Vader", "Impressive. Most impressive." },
    { "Darth Vader", "If he could be turned, he would become a powerful ally." },
    { "Darth Vader", "When I left you, I was but the learner. Now I am the master." },
    { "Darth Vader", "All too easy." },
    { "Darth Vader", "You have failed me for the last time, Admiral." },
    { "Darth Vader", "The Emperor is not as forgiving as I am." },
    { "Darth Vader", "You would be wise to surrender." },
    { "Darth Vader", "No. I am your father." },
    { "Darth Vader", "Search your feelings. You know it to be true." },
    { "Darth Vader", "If you only knew the power of the dark side." },

    { "Darth Maul", "He lives... he lives!" },
    { "Darth Maul", "I can see him in my mind's eye. Kenobi. Kenobi!" },
    { "Darth Maul", "Revenge. I must have revenge." },
    { "Darth Maul", "Look what has become of you. A rat in the desert." },
    { "Darth Maul", "He will avenge us." },

    { "Darth Sidious", "Strike me down with all of your hatred and your journey toward the dark side will be complete!" },
    { "Darth Sidious", "Young fool. Only now, at the end, do you understand." },
    { "Darth Sidious", "Let the hate flow through you." },
    { "Darth Sidious", "Good! Your hate has made you powerful." },
    { "Darth Sidious", "In time, you will call me Master." },
    { "Darth Sidious", "Everything is proceeding as I have foreseen." },
    { "Darth Sidious", "The Jedi are dead!" },
    { "Darth Sidious", "You are nothing!" },
};

// The Bendu, between the light and the dark, can appear in every holocron.
inline constexpr Quote bendu_quotes[] = {
    { "Bendu", "Jedi and Sith wield the Ashla and Bogan, the light and the dark. I'm the one in the middle, the Bendu." },
    { "Bendu", "An object cannot make you good or evil. The temptation of power, forbidden knowledge, even the desire to do good can lead some down that path. But only you can change yourself." },
    { "Bendu", "You carry conflict with you, Kanan Jarrus, Jedi Knight." },
    { "Bendu", "I am beyond your worrying and wars. I am unseen, unknowable, like a rock in the river." },
    { "Bendu", "I see your defeat, like many arms surrounding you in a cold embrace." },
};

// /holocron draws from every quote; /holocron_jedi and /holocron_sith from
// their own side plus the Bendu.
inline const Quote& random_quote(std::string_view command) {
    const std::span<const Quote> none;
    const std::span<const Quote> pools[] = {
        command != "holocron_sith" ? std::span<const Quote>(jedi_quotes) : none,
        command != "holocron_jedi" ? std::span<const Quote>(sith_quotes) : none,
        bendu_quotes,
    };
    std::size_t total = 0;
    for (std::span<const Quote> pool : pools) total += pool.size();

    thread_local std::mt19937 rng{std::random_device{}()};
    std::size_t pick = std::uniform_int_distribution<std::size_t>(0, total - 1)(rng);
    for (std::span<const Quote> pool : pools) {
        if (pick < pool.size()) return pool[pick];
        pick -= pool.size();
    }
    return bendu_quotes[0];  // unreachable: pick < total
}

// ------------------------------------------------------------------- replies

// Every holocron carries a button that opens another from the same command.
// Its id is this prefix plus the command, so the bot keeps no state for it.
inline constexpr std::string_view holocron_prefix = "holocron:";

inline bool is_holocron_command(std::string_view command) {
    return command == "holocron" || command == "holocron_jedi" || command == "holocron_sith";
}

inline dpp::message holocron_message(const Quote& picked, const std::string& command) {
    const HolocronLook look = holocron_look(picked.author, command);

    dpp::message message;
    message.add_embed(dpp::embed()
        .set_title(theme::titled(look.icon, look.name))
        .set_description("> *\"" + std::string(picked.text) + "\"*\n\n" + std::string(look.book) + " **" + std::string(picked.author) + "**")
        .set_color(look.color)
        .set_thumbnail(look.image));
    message.add_component(dpp::component().add_component(dpp::component()
        .set_label("Open Another")
        .set_emoji(std::string(command == "holocron_sith" ? theme::icon::sith : theme::icon::jedi))
        .set_style(dpp::cos_secondary)
        .set_id(std::string(holocron_prefix) + command)));
    return message;
}

// Answers both the slash command and the button under an earlier holocron.
inline void holocron_command(const dpp::interaction_create_t& event, const std::string& command) {
    event.reply(holocron_message(random_quote(command), command));
}
