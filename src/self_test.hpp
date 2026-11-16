// self_test.hpp
//
// `AurebeshDroid --self-test [folder]` checks everything that can be checked
// without Discord: every script and style draws, every reply and control fits
// Discord's limits, and every id survives the trip through a button. Given a
// folder, it also saves sample images there to look at.

#pragma once

// Prints one line per check and returns 0 if they all pass, 1 if not.
int self_test(const char* folder);
