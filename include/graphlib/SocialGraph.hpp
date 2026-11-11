//
//  File:   SocialGraph.hpp
//  Purpose:
//  Builds a 20-person social graph that includes me and Kevin Bacon.
//  Friendship is mutual, so every link is added in both directions.
//  Works with any Graph<std::string> representation.
//
//  Layout:
//    Harper Hatcher - Maya Chen - Sam Rivera ----.
//    Harper Hatcher - Jordan Brooks - Olivia Hart - Grace Turner (film extra)
//    Harper Hatcher - Priya Patel - Noah Kim - Liam Foster - Grace Turner
//    Grace Turner - Meg Ryan
//    Meg Ryan - Tom Hanks       (Sleepless in Seattle)
//    Meg Ryan - Tom Cruise      (Top Gun)
//    Tom Hanks - Kevin Bacon    (Apollo 13)
//    Kevin Bacon - Tom Cruise / Demi Moore / Jack Nicholson (A Few Good Men)
//    Tom Cruise - Demi Moore    (A Few Good Men)
//
//    Separate group, not connected to anyone above (a 5-person ring):
//    Zoe Adams - Ben Carter - Chloe Nguyen - Ryan Scott - Ava Morales - Zoe Adams
//
#pragma once

#include "Graph.hpp"

#include <string>
#include <utility>
#include <vector>

inline const std::vector<std::string> SOCIAL_PEOPLE = {
    "Harper Hatcher",     "Maya Chen",   "Jordan Brooks", "Priya Patel",  "Sam Rivera",
    "Olivia Hart",  "Noah Kim",    "Liam Foster",   "Grace Turner", "Meg Ryan",
    "Tom Hanks",    "Tom Cruise",  "Demi Moore",    "Jack Nicholson", "Kevin Bacon",
    "Zoe Adams",    "Ben Carter",  "Chloe Nguyen",  "Ryan Scott",   "Ava Morales"};

inline const std::vector<std::pair<std::string, std::string>> SOCIAL_FRIENDSHIPS = {
    {"Harper Hatcher", "Maya Chen"},
    {"Harper Hatcher", "Jordan Brooks"},
    {"Harper Hatcher", "Priya Patel"},
    {"Maya Chen", "Sam Rivera"},
    {"Sam Rivera", "Olivia Hart"},
    {"Jordan Brooks", "Olivia Hart"},
    {"Priya Patel", "Noah Kim"},
    {"Noah Kim", "Liam Foster"},
    {"Olivia Hart", "Grace Turner"},
    {"Liam Foster", "Grace Turner"},
    {"Grace Turner", "Meg Ryan"},
    {"Meg Ryan", "Tom Hanks"},
    {"Meg Ryan", "Tom Cruise"},
    {"Tom Hanks", "Kevin Bacon"},
    {"Kevin Bacon", "Tom Cruise"},
    {"Kevin Bacon", "Demi Moore"},
    {"Kevin Bacon", "Jack Nicholson"},
    {"Tom Cruise", "Demi Moore"},
    // disconnected ring
    {"Zoe Adams", "Ben Carter"},
    {"Ben Carter", "Chloe Nguyen"},
    {"Chloe Nguyen", "Ryan Scott"},
    {"Ryan Scott", "Ava Morales"},
    {"Ava Morales", "Zoe Adams"},
};

// Fills `g` with the social graph. `g` should start empty.
inline void buildSocialGraph(Graph<std::string>& g) {
    for (const auto& person : SOCIAL_PEOPLE) g.addNode(person);
    for (const auto& [a, b] : SOCIAL_FRIENDSHIPS) {
        g.addEdge(a, b);
        g.addEdge(b, a);
    }
}
