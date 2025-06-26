// holocron.hpp

#pragma once

#include <string>
#include <vector>

using namespace std;

bool contains(const string& text1, const string& text2) {
    return text1.find(text2) != string::npos;
}

struct HolocronInfo {
    string title;
    uint32_t color;
    string book;
    string image;
};

HolocronInfo get_holocron_metadata(const string& author, const string& command) {
    string blue = "https://raw.githubusercontent.com/TheAbubakrAbu/Aurebesh-Droid/main/src/assets/images/aurebesh.png";
    string green = "https://raw.githubusercontent.com/TheAbubakrAbu/Aurebesh-Droid/main/src/assets/images/green.png";
    string yellow = "https://raw.githubusercontent.com/TheAbubakrAbu/Aurebesh-Droid/main/src/assets/images/yellow.png";
    string purple = "https://raw.githubusercontent.com/TheAbubakrAbu/Aurebesh-Droid/main/src/assets/images/purple.png";
    //string red = "https://raw.githubusercontent.com/TheAbubakrAbu/Aurebesh-Droid/main/src/assets/images/red.png";
    string bendu = "https://static.wikia.nocookie.net/starwars/images/3/34/Bendu.png/revision/latest?cb=20230312180517";

    if (author == "Bendu") {
        return { "🔸 Bendu's Wisdom 🔸", 0xDAA520, "📙", bendu };
    } else if(contains(command, "sith") || contains(author, "Darth") || contains(author, "Count")) {
        return { "🔺 Sith Holocron 🔺", 0x8B0000, "📕", blue };
    } else if(contains(author, "Luke") || contains(author, "Qui") || contains(author, "Yoda")) {
        return { "💠 Jedi Holocron 💠", 0x228B22, "📗", green };
    } else if (contains(author, "Mace")) {
        return { "💠 Jedi Holocron 💠", 0x800080, "📘📕", purple };
    } else if (contains(author, "Temple")) {
        return { "💠 Jedi Holocron 💠", 0xFFD700, "📒", yellow };
    } else {
        return { "💠 Jedi Holocron 💠", 0x4295E2, "📘", blue };
    }
}

vector<pair<string, string>> jedi_quotes = {
    // Prequel Jedi Quotes
    { "Yoda", "Fear is the path to the dark side. Fear leads to anger. Anger leads to hate. Hate leads to suffering." },
    { "Yoda", "Train yourself to let go of everything you fear to lose." },
    { "Yoda", "The dark side clouds everything. Impossible to see, the future is." },
    { "Yoda", "Truly wonderful, the mind of a child is." },
    { "Yoda", "A Jedi uses the Force for knowledge and defense, never for attack." },
    { "Yoda", "You must unlearn what you have learned." },
    { "Yoda", "Difficult to see. Always in motion is the future." },
    { "Yoda", "To be Jedi is to face the truth, and choose." },
    { "Yoda", "The shroud of the dark side has fallen. Begun, the Clone War has." },
    { "Yoda", "Much to learn, you still have." },
    { "Yoda", "Size matters not. Look at me. Judge me by my size, do you?" },
    { "Yoda", "Adventure. Excitement. A Jedi craves not these things." },
    { "Yoda", "In the end, cowards are those who follow the dark side." },
    { "Yoda", "A challenge lifelong it is, not to bend fear into anger." },

    { "Mace Windu", "This party’s over." },
    { "Mace Windu", "The oppression of the Sith will never return. You have lost." },
    { "Mace Windu", "I sense a plot to destroy the Jedi." },
    { "Mace Windu", "If what you've told me is true, you will have gained my trust." },
    { "Mace Windu", "The dark side of the Force surrounds the Chancellor." },
    { "Mace Windu", "We are protectors of peace, not soldiers." },

    { "Obi-Wan Kenobi", "You were the Chosen One! It was said that you would destroy the Sith, not join them!" },
    { "Obi-Wan Kenobi", "Only a Sith deals in absolutes." },
    { "Obi-Wan Kenobi", "You have allowed this Dark Lord to twist your mind until now... you have become the very thing you swore to destroy." },
    { "Obi-Wan Kenobi", "So uncivilized." },
    { "Obi-Wan Kenobi", "Anakin, my allegiance is to the Republic, to democracy!" },
    { "Obi-Wan Kenobi", "I have failed you, Anakin. I have failed you." },
    { "Obi-Wan Kenobi", "Another happy landing." },
    { "Obi-Wan Kenobi", "Your lightsabers will make a fine addition to my collection." },
    { "Obi-Wan Kenobi", "Why do I get the feeling you're going to be the death of me?" },
    { "Obi-Wan Kenobi", "Good job." },
    { "Obi-Wan Kenobi", "The Council isn't always right. That's why I'm asking for your help." },

    { "Qui-Gon Jinn", "Feel, don't think. Use your instincts." },
    { "Qui-Gon Jinn", "Your focus determines your reality." },
    { "Qui-Gon Jinn", "The ability to speak does not make you intelligent." },
    { "Qui-Gon Jinn", "Remember: concentrate on the moment. Feel, don't think. Trust your instincts." },
    { "Qui-Gon Jinn", "There's always a bigger fish." },
    { "Qui-Gon Jinn", "I will do what I must, Obi-Wan." },
    { "Qui-Gon Jinn", "He is the Chosen One. He will bring balance." },
    { "Qui-Gon Jinn", "Balance is not letting your emotions control you." },

    { "Anakin Skywalker", "The biggest problem in the universe is that nobody helps each other." },
    { "Anakin Skywalker", "I’m a Jedi. I know I’m better than this." },
    { "Anakin Skywalker", "Someday I will be the most powerful Jedi ever!" },
    { "Anakin Skywalker", "I want more. And I know I shouldn't." },
    { "Anakin Skywalker", "Sometimes, we must let go of our pride and do what is requested of us." },
    { "Anakin Skywalker", "I'm a Jedi, I know I'm better than this!" },

    { "Ki-Adi-Mundi", "What about the droid attack on the Wookiees?" },
    { "Ki-Adi-Mundi", "The Sith have been extinct for a millennium." },

    { "Plo Koon", "The Force will guide us, as it always has." },

    { "Jedi Temple Guard", "You have made your choice." },
    { "Jedi Temple Guard", "The Jedi Temple has no place for you." },
    { "Jedi Temple Guard", "You know what must be done." },
    { "Jedi Temple Guard", "A Jedi must be willing to face the truth, no matter how painful." },
    { "Jedi Temple Guard", "The future is not given. You must choose your path." },

    // Original Jedi Quotes
    { "Obi-Wan Kenobi", "Use the Force, Luke." },
    { "Obi-Wan Kenobi", "You can't win, Darth. If you strike me down, I shall become more powerful than you can possibly imagine." },
    { "Obi-Wan Kenobi", "The Force will be with you, always." },
    { "Obi-Wan Kenobi", "This is the weapon of a Jedi Knight. Not as clumsy or random as a blaster; an elegant weapon for a more civilized age." },
    { "Obi-Wan Kenobi", "Mos Eisley spaceport. You will never find a more wretched hive of scum and villainy. We must be cautious." },
    { "Obi-Wan Kenobi", "Your eyes can deceive you. Don’t trust them." },
    
    { "Luke Skywalker", "I am a Jedi, like my father before me." },
    { "Luke Skywalker", "I'll never turn to the dark side. You've failed, Your Highness." },
    { "Luke Skywalker", "I feel the good in you. The conflict." },
    { "Luke Skywalker", "I won't fight you, Father." },
    { "Luke Skywalker", "I have accepted the truth that you were once Anakin Skywalker, my father." },
    { "Luke Skywalker", "Search your feelings, Father. You can't do this. I feel the conflict within you." },
    { "Luke Skywalker", "Confronting fear is the destiny of the Jedi." },

    { "Yoda", "Judge me by my size, do you?" },
    { "Yoda", "You must unlearn what you have learned." },
    { "Yoda", "Luminous beings are we, not this crude matter." },
    { "Yoda", "Wars not make one great." },
    { "Yoda", "For my ally is the Force. And a powerful ally it is." },
    { "Yoda", "Do or do not. There is no try." },
    { "Yoda", "When you look at the dark side, careful you must be. For the dark side looks back." },
    { "Yoda", "Always in motion is the future." },
    { "Yoda", "A Jedi uses the Force for knowledge and defense, never for attack." },
    { "Yoda", "That is why you fail." },
    { "Yoda", "The greatest teacher, failure is." },

    // Rebels Jedi Quotes
    { "Kanan Jarrus", "I survived one war. I’m not ready for another." },
    { "Kanan Jarrus", "In my experience, when you think you understand the Force, you find out how little you actually know." },
    { "Kanan Jarrus", "The Jedi didn’t start the war, but they were trained to fight it." },
    { "Kanan Jarrus", "A Jedi trusts the Force, not a weapon." },
    { "Kanan Jarrus", "I have nothing left to fear." },
    { "Kanan Jarrus", "The Force is in all of us. Even you’ve never been more connected than you are now." },
    { "Kanan Jarrus", "You are not alone, Ezra." },
    { "Kanan Jarrus", "You’ll never be strong enough until you overcome the fear inside you." },
    { "Kanan Jarrus", "When we’re together, there’s nothing we can’t do." },
    { "Kanan Jarrus", "There’s always a way for those who are willing to act." },

    { "Ezra Bridger", "I’m going to do something. I have to help." },
    { "Ezra Bridger", "We have hope. Hope that things can get better." },
    { "Ezra Bridger", "The Force is trying to tell me something." },
    { "Ezra Bridger", "I never said I was a Jedi." },
    { "Ezra Bridger", "I may not be able to protect Lothal forever, but I can give them a chance." },
    { "Ezra Bridger", "The Emperor will never have me." },
    { "Ezra Bridger", "This is about Lothal. It’s about my home." },
    { "Ezra Bridger", "I’m not afraid. I know what I have to do now." },
    { "Ezra Bridger", "Sometimes, you have to stand up even when it’s hard." },

    // Fallen Order Jedi Quotes
    { "Cal Kestis", "Trust only in the Force." },
    { "Cal Kestis", "The galaxy may need a new order. One built on freedom and hope." },
    { "Cal Kestis", "We do what we must to survive. But we don’t give up." },
    { "Cal Kestis", "This is the path of the Jedi. It’s not easy." },
    { "Cal Kestis", "I will never forget. The Empire took everything from me." },
    { "Cal Kestis", "I’m not afraid of who I was anymore." },
    { "Cal Kestis", "We’re all connected through the Force." },
    { "Cal Kestis", "We can't change the past, but we can fight for the future." },
    { "Cal Kestis", "BD, we’ve got to keep going. We’re not alone." },
    { "Cal Kestis", "I still believe in what the Jedi stood for." },
};

vector<pair<string, string>> sith_quotes = {
    // Prequels Sith Quotes
    { "Darth Sidious", "The dark side of the Force is a pathway to many abilities some consider to be unnatural." },
    { "Darth Sidious", "There is only one truth, Anakin—me." },
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
    { "Count Dooku", "You disappoint me, Yoda. You have become weak and misled by the Jedi Council." },
    { "Count Dooku", "I sense great fear in you, Skywalker. You have hate. You have anger. But you don't use them." },
    { "Count Dooku", "It is obvious that this contest cannot be decided by our knowledge of the Force... but by our skills with a lightsaber." },
    { "Count Dooku", "You have courage. But foolishness as well." },

    { "Darth Maul", "At last we will reveal ourselves to the Jedi. At last we will have revenge." },
    { "Darth Maul", "Fear. Fear attracts the fearful. The strong. The weak. The innocent. The corrupt. Fear is my ally." },
    { "Darth Maul", "I was hoping for Kenobi. Why are you here?" },
    { "Darth Maul", "You may have forgotten me... but I will never forget you." },
    { "Darth Maul", "The Jedi are dead! But there is another Skywalker." },
    { "Darth Maul", "There is no justice, no law, no order, except for the one that will replace it!" },

    { "Darth Vader", "You underestimate my power!" },
    { "Darth Vader", "If you're not with me, then you're my enemy." },
    { "Darth Vader", "From my point of view, the Jedi are evil!" },
    { "Darth Vader", "You will not take her from me!" },
    { "Darth Vader", "I have brought peace, freedom, justice, and security to my new empire." },

    // Original Sith Quotes
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
    { "Darth Vader", "The Jedi are dead!" },
    { "Darth Vader", "There are no more Jedi." },
    { "Darth Vader", "You are nothing." },
    { "Darth Vader", "You would be wise to surrender." },

    { "Darth Maul", "He lives... he lives!" },
    { "Darth Maul", "You’re in the wrong place, Ezra Bridger." },
    { "Darth Maul", "The Jedi and Sith… the light and the dark… I’m beyond that now." },
    { "Darth Maul", "I see him… Kenobi!" },
    { "Darth Maul", "Revenge. I must have revenge." },

    { "Darth Sidious", "Strike me down with all of your hatred and your journey toward the dark side will be complete!" },
    { "Darth Sidious", "Young fool. Only now, at the end, do you understand." },
    { "Darth Sidious", "Let the hate flow through you." },
    { "Darth Sidious", "Good! Your hate has made you powerful." },
    { "Darth Sidious", "Soon you will call me Master." },
};