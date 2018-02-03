/**********************************************************
 * @file   Overloaded.h
 * @author jan
 * @date   7/15/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_OVERLOADED_H
#define NOVELIST_OVERLOADED_H

#include <type_traits>
#include <utility>

namespace novelist {

    /**
     * Overloaded callable
     * @tparam Ts A number of callable types
     */
    template<class... Ts>
    struct Overloaded : Ts ... {
        constexpr Overloaded(Ts ... ts) noexcept((std::is_nothrow_move_constructible_v<Ts> && ...))
                :Ts(std::move(ts))... { }

        using Ts::operator()...;
    };

    template<class... Ts> Overloaded(Ts...) -> Overloaded<Ts...>;

}

#endif //NOVELIST_OVERLOADED_H
