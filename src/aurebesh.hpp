// aurebesh.hpp

#ifndef AUREBESH_HPP
#define AUREBESH_HPP

#include <string>

using namespace std;

struct AurebeshLetter {    
    string aurebeshName;
    string letter;
};

int aurebeshSize = 26;

AurebeshLetter aurebeshLetters[26] {
    {"Aurek", "A"},
    {"Besh",  "B"},
    {"Cresh", "C"},
    {"Dorn",  "D"},
    {"Esk",   "E"},
    {"Forn",  "F"},
    {"Grek",  "G"},
    {"Herf",  "H"},
    {"Isk",   "I"},
    {"Jenth", "J"},
    {"Krill", "K"},
    {"Leth",  "L"},
    {"Mern",  "M"},
    {"Nern",  "N"},
    {"Osk",   "O"},
    {"Peth",  "P"},
    {"Qek",   "Q"},
    {"Resh",  "R"},
    {"Senth", "S"},
    {"Trill", "T"},
    {"Usk",   "U"},
    {"Vev",   "V"},
    {"Wesk",  "W"},
    {"Xesh",  "X"},
    {"Yirt",  "Y"},
    {"Zerek", "Z"},
};

#endif