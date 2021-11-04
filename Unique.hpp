#pragma once

struct Unique {
    Unique() = default;
    Unique(Unique const&) = delete;
    Unique(Unique&&) = delete;
    Unique& operator=(Unique const&) = delete;
    Unique& operator=(Unique&&) = delete;
};
