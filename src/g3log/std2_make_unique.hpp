/** ==========================================================================
 * 2013 This is PUBLIC DOMAIN to use at your own risk and comes
 * with no warranties. This code is yours to share, use and modify with no
 * strings attached and no restrictions or obligations.
 *
 * For more information see g3log/LICENSE or refer refer to http://unlicense.org
 *
 * make_unique will be in C++14, this implementation is copied as I understood
 * Stephan T. Lavavej's description of it.
 *
 * PUBLIC DOMAIN and NOT under copywrite protection.
 *
 *
 * Example: usage
 * auto an_int = make_unique<int>(123);
 * auto a_string = make_unique<string>(5, 'x');
 * auto an_int_array = make_unique<int[]>(11, 22, 33);
 * ********************************************* */

#pragma once

#include <memory>
#include <type_traits>

namespace std2 {
   namespace impl_fut_stl {

      template<typename T, typename ... Args>
      std::unique_ptr<T> make_unique_helper(std::false_type, Args &&... args) {
         return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
      }

      template<typename T, typename ... Args>
      std::unique_ptr<T> make_unique_helper(std::true_type, Args &&... args) {
         static_assert(std::extent<T>::value == 0, "make_unique<T[N]>() is forbidden, please use make_unique<T[]>(),");
         typedef typename std::remove_extent<T>::type U;
         return std::unique_ptr<T>(new U[sizeof...(Args)] {std::forward<Args>(args)...});
      }
   }

   template<typename T, typename ... Args>
   std::unique_ptr<T> make_unique(Args &&... args) {
      return impl_fut_stl::make_unique_helper<T>(
                std::is_array<T>(), std::forward<Args>(args)...);
   }
}

