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

namespace novelist {

    /**
     * Overloaded callable
     * @tparam Ts A number of callable types
     */
    template<class... Ts>
    struct Overloaded : Ts ... {
        using Ts::operator()...;
    };

    /**
     * Overloaded callable
     * @tparam Ts A number of callable types
     */
    template<class... Ts> Overloaded(Ts ...) -> Overloaded<Ts...>;
}

#endif //NOVELIST_OVERLOADED_H
