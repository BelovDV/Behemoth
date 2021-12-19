#include <gtest/gtest.h>

#include "suffix_automat.hpp"

char generate_random_letter()
{
    return rand() % ('z' - 'a') + 'a';
}

// ===== // ===== // ===== // ===== // ===== // ===== // ===== // ===== //
// testing runtime errors

TEST(Fails, construction)
{
    auto automat = Suffix_automat<>();
    std::string input_1("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
    automat.insert(input_1);
    EXPECT_EQ(automat.size(), input_1.size() + 1); // + root
    std::string input_2("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb");
    automat.insert(input_2);
    EXPECT_EQ(automat.size(), input_1.size() + input_2.size() + 1);
    automat.insert(std::string("ccdcd"));
    EXPECT_EQ(automat.size(), input_1.size() + input_2.size() + 1 + 6);
}

TEST(Fails, walk)
{
    std::string input;
    for (size_t i = 0; i < 10000; ++i)
        input.push_back(generate_random_letter());
    auto automat = Suffix_automat<>(input);
    auto state = automat.root();
    for (size_t i = 0; i < 100000; ++i)
    {
        state.move(generate_random_letter());
        if (rand() % 100)
            state.move_suffix();
    }
}

// ===== // ===== // ===== // ===== // ===== // ===== // ===== // ===== //
// testing structure of automat

#include <unordered_set>

TEST(Correct, cycles)
{
    std::string input;
    for (size_t i = 0; i < 1000; ++i)
        input.push_back(generate_random_letter());
    auto automat = Suffix_automat<>(input);
    for (size_t i = 0; i < 1000; ++i)
    {
        std::unordered_set<unsigned> visited;
        auto state = automat.root();
        bool is_way = false;
        while (is_way)
        {
            is_way = false;
            for (size_t j = 0; j < 100; ++j)
            {
                auto c = generate_random_letter();
                if (state.is_way(c))
                {
                    auto next = state.next(c);
                    EXPECT_EQ(visited.count(next), 0);
                    visited.insert(next);
                    state.move(c);
                    is_way = true;
                    break;
                }
            }
        }
    }
}

TEST(Correct, suffixes)
{
    std::string input;
    for (size_t i = 0; i < 1000; ++i)
        input.push_back(generate_random_letter());
    auto automat = Suffix_automat<>(input);
    for (size_t i = 0; i < 1000; ++i)
    {
        std::unordered_set<unsigned> visited;
        auto state = automat.root();
        bool is_way = false;
        while (is_way)
        {
            is_way = false;
            for (size_t j = 0; j < 100; ++j)
            {
                auto c = generate_random_letter();
                if (state.is_way(c))
                {
                    auto next = state.next(c);
                    visited.insert(next);
                    state.move(c);
                    is_way = true;
                    break;
                }
            }
        }
        while (state.suffix() != state.position())
            state.move_suffix();
        EXPECT_EQ(state.position(), 0);
    }
}

// ===== // ===== // ===== // ===== // ===== // ===== // ===== // ===== //
// testing substings

TEST(Substrings, all)
{
    std::string input;
    for (size_t i = 0; i < 100; ++i)
        input.push_back(generate_random_letter());
    auto automat = Suffix_automat<>(input);
    auto state = automat.root();
    for (auto begin = input.begin(); begin != input.end(); ++begin)
        for (auto end = begin + 1; end <= input.end(); ++end)
        {
            auto state = automat.root();
            for (auto iter = begin; iter != end; ++iter) {
                EXPECT_EQ(state.is_way(*iter), true);
                state.move(*iter);
            }
        }
}

// ===== // ===== // ===== // ===== // ===== // ===== // ===== // ===== //

TEST(CI, error)
{
    ASSERT_EQ(1, 0);
}

int main(int argc, char **argv)
{
    srand(0);
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
