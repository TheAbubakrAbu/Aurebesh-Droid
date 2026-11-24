# Discord Listing

The text on Aurebesh Droid's pages in the Discord Developer Portal, kept here so it can be restored or matched by the other bots ([Sabacc Droid](https://github.com/TheAbubakrAbu/Sabacc-Droid/blob/main/docs/discord-listing.md) and [501st War Room](https://github.com/TheAbubakrAbu/501st-War-Room/blob/main/docs/discord-listing.md)).

## General Information

**Description** (maximum 400 characters, shown in the bot's About Me):

```
🔤 Translate English into Aurebesh and 13 more Star Wars scripts, right in your server!
🎨 Restyle any translation with new colors and backgrounds
🔷 Open Jedi and Sith holocrons for hidden wisdom
🧠 Test your crew with the Aurebesh /quiz
✨ Start with /translate · See it all with /help

Created by Abubakr Elmallah
📧 contact@abubakrelmallah.com
🌐 https://abubakrelmallah.com/star-wars/
```

**Tags** (maximum 5, also the App Directory tags): Star Wars, Aurebesh, Translator, Roleplay, Entertainment

**Terms of Service URL** and **Privacy Policy URL:** https://abubakrelmallah.com/star-wars/

## Installation

- **Installation Contexts:** User Install and Guild Install
- **Install Link:** Discord Provided Link
- **User Install scopes:** applications.commands
- **Guild Install scopes:** applications.commands, bot
- **Guild Install permissions:** Send Messages, Send Messages in Threads, Use Slash Commands, View Channels. Every reply is an interaction response, which needs no channel permissions, so this is enough.

With User Install on, `commands.hpp` registers every command for personal installs too, so people can use the bot in any server, DM, or group DM.

## OAuth2

Nothing set: no redirects, and Public Client is off.

## Bot

- **Icon:** the Aurebesh Droid logo
- **Banner:** [`banner.png`](../banner.png) (1360 x 480, Discord's 17:6 at twice the size): the name in Aurebesh and English, with the bot's Sith and Jedi holocrons. The earlier holocron picture is kept as [`src/assets/images/banner-old.jpg`](../src/assets/images/banner-old.jpg).
- **Username:** Aurebesh Droid
- **Status:** "Playing 🔤 Aurebesh", set in code in `main.cpp`
- **Public Bot:** on
- **Requires OAuth2 Code Grant:** off
- **Privileged Gateway Intents:** all off (Presence, Server Members, Message Content)

## Emojis and Rich Presence

- **Emojis:** none uploaded. Every icon is a Unicode emoji from `theme.hpp`.
- **Rich Presence:** not used. Rich Presence art is for games that report what a player is doing; a bot's status is set in code.

## Discovery

**Discovery Status:** on (the bot is listed in the App Directory)

**Summary** (maximum 200 characters):

```
Translate English into Aurebesh and 13 other Star Wars scripts right in Discord! Explore the galactic alphabet and unlock ancient Jedi and Sith wisdom through interactive holocrons.
```

**Languages:** English, US

**Support Server:** The Skywalker Order

**Media** (1920 x 1080, made from screenshots of the bot in a DM, in this order):

1. [`media/1-translate.png`](media/1-translate.png): Write in Aurebesh (`/translate`)
2. [`media/2-customize.png`](media/2-customize.png): Restyle it your way (the Customize controls, in Sith Script and Sith Red)
3. [`media/3-holocrons.png`](media/3-holocrons.png): Open a holocron (`/holocron_jedi` and `/holocron_sith`)
4. [`media/4-quiz.png`](media/4-quiz.png): Quiz your server (`/quiz`)
5. [`media/5-alphabet.png`](media/5-alphabet.png): Learn every letter (`/alphabet`)

Each is linked in the portal as `https://raw.githubusercontent.com/TheAbubakrAbu/Aurebesh-Droid/main/docs/media/<file>`, so renaming or moving one breaks its slot in the listing.

**Links:**

- Website: https://abubakrelmallah.com/star-wars/
- GitHub: https://github.com/TheAbubakrAbu/Aurebesh-Droid
- Try Sabacc Droid: https://discord.ly/sabaac-droid

**Expanded Description** (the portal's Markdown has no `#` headings, so the title is bold):

```
**Aurebesh Droid | Your Star Wars Translator & Holocron Companion!**

Bring the languages and mysteries of a galaxy far, far away to your Discord server! Translate English into Aurebesh and 13 additional Star Wars scripts, explore the galactic alphabet, and discover ancient wisdom hidden within Jedi and Sith holocrons.

🌌 **Features**

• **Galactic Translation:** Convert English text into Aurebesh and 13 other Star Wars scripts.

• **High-Quality Images:** Generate translated text as images using authentic Star Wars-inspired lettering.

• **Aurebesh Alphabet:** Explore the iconic writing system used throughout the Star Wars galaxy.

• **Jedi Holocrons:** Unlock randomized teachings and wisdom from the Jedi Order.

• **Sith Holocrons:** Discover the secrets and philosophy of the dark side.

• **Easy to Use:** Access everything directly through Discord slash commands.

🤖 **Commands**

`/translate` - Translate English into Aurebesh.

`/aurebesh` or `/alphabet` - View the Aurebesh alphabet.

`/holocron` - Open a mysterious holocron.

`/holocron_jedi` - Discover Jedi wisdom.

`/holocron_sith` - Uncover Sith teachings.

Whether you're a Jedi, Sith, Mandalorian, or simply a Star Wars enthusiast, Aurebesh Droid brings a piece of the galaxy into your server.

May the Force be with you!

Created by Abubakr Elmallah

Website: https://abubakrelmallah.com/star-wars/

Support: contact@abubakrelmallah.com
```
