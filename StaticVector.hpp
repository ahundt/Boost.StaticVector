/**
 * @file   StaticVector.hpp
 * @date   Feb 23, 2011
 * @brief  Vector style class with fixed capacity.
 *
 * The following code declares class StaticVector,
 * an STL container (as wrapper) for a statically allocated vector with a constant size limit.
 *  StaticVector is not accepted as part of boost.
 *
 * (C) Andrew Hundt 2011
 * (C) Carnegie Mellon University 2011
 * @author Andrew Hundt <ahundt@cmu.edu>
 *
 * some portions are based on AutoBuffer
 * (C) Copyright Thorsten Ottosen, 2009.
 *
 * based on boost::array
 * The original author site of boost::array is at: http://www.josuttis.com/
 * (C) Copyright Nicolai M. Josuttis 2001.
 *
 *
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * 09 Oct 2011 - (ath) eliminated construction of objects on initialization of StaticVector
 * 23 Feb 2011 - (ath) converted to boost::StaticVector
 * 28 Dec 2010 - (mtc) Added cbegin and cend (and crbegin and crend) for C++Ox compatibility.
 * 10 Mar 2010 - (mtc) fill method added, matching resolution of the standard library working group.
 *      See <http://www.open-std.org/jtc1/sc22/wg21/docs/lwg-defects.html#776> or Trac issue #3168
 *      Eventually, we should remove "assign" which is now a synonym for "fill" (Marshall Clow)
 * 10 Mar 2010 - added workaround for SUNCC and !STLPort [trac #3893] (Marshall Clow)
 * 29 Jan 2004 - c_array() added, BOOST_NO_PRIVATE_IN_AGGREGATE removed (Nico Josuttis)
 * 23 Aug 2002 - fix for Non-MSVC compilers combined with MSVC libraries.
 * 05 Aug 2001 - minor update (Nico Josuttis)
 * 20 Jan 2001 - STLport fix (Beman Dawes)
 * 29 Sep 2000 - Initial Revision (Nico Josuttis)
 *
 * Jan 29, 2004
 */

#ifndef BOOST_STATIC_VECTOR_HPP
#define BOOST_STATIC_VECTOR_HPP

#include <boost/config.hpp>
#include <boost/detail/workaround.hpp>

#if BOOST_WORKAROUND(BOOST_MSVC, >= 1400)  
# pragma warning(push)  
# pragma warning(disable:4996) // 'std::equal': Function call with parameters that may be unsafe
# pragma warning(disable:4510) // boost::StaticVector<T,N>' : default constructor could not be generated
# pragma warning(disable:4610) // warning C4610: class 'boost::StaticVector<T,N>' can never be instantiated - user defined constructor required
#endif

#include <cstddef>
#include <stdexcept>
#include <boost/assert.hpp>
#if ((BOOST_VERSION / 100) % 1000) > 44
#include <boost/swap.hpp>
#endif
#include <boost/iterator/reverse_iterator.hpp>
#include <boost/iterator/iterator_traits.hpp>
// Handles broken standard libraries better than <iterator>
#include <boost/detail/iterator.hpp>
#include <boost/throw_exception.hpp>
#include <algorithm>

// FIXES for broken compilers
#include <boost/config.hpp>

// Selection of types for internal storage
#include <boost/integer.hpp>
#include <boost/type_traits/alignment_of.hpp>
#include <boost/type_traits/aligned_storage.hpp>
#include <boost/type_traits/has_nothrow_copy.hpp>
#include <boost/type_traits/has_nothrow_assign.hpp>
#include <boost/type_traits/has_trivial_assign.hpp>
#include <boost/type_traits/has_trivial_constructor.hpp>
#include <boost/type_traits/has_trivial_destructor.hpp>


namespace boost {
 /* template<class T, std::size_t N>
  class StaticVector;*/

  namespace detail { // forward declarations
        template< typename InputIterator, typename OutputIterator >
        static void uninitialized_copy_backward(InputIterator begin, InputIterator end, OutputIterator result);
  }

    template<class T, std::size_t N>
    class StaticVector {
      public:
        // type definitions
        typedef T                                              value_type;
        typedef T*                                             pointer;
        typedef const T*                                       const_pointer;
        typedef T*                                             iterator;
        typedef const T*                                       const_iterator;
        typedef T&                                             reference;
        typedef const T&                                       const_reference;
        typedef typename boost::aligned_storage<               
                           sizeof(T),                          
                           boost::alignment_of<T>::value       
                         >::type                               aligned_storage;
        typedef typename boost::uint_value_t<N>::least         size_type;
        typedef std::size_t                                    max_size_type;
        typedef std::ptrdiff_t                                 difference_type;
        typedef typename boost::mpl::if_c< boost::has_trivial_assign<T>::value
                                           && sizeof(T) <= sizeof(long double),
                                          const value_type, 
                                          const_reference >::type
                                                      optimized_const_reference;
        
      private:
        size_type m_size; // fastest type that can accomodate N
        aligned_storage elems[N];    // fixed-size array of memory aligned elements of type T

      public:

        // iterator support
        iterator        begin()       { return reinterpret_cast<iterator>(elems); }
        const_iterator  begin() const { return reinterpret_cast<const_iterator>(elems); }
        const_iterator cbegin() const { return reinterpret_cast<const_iterator>(elems); }
        
        iterator        end()       { return to_object(m_size); }
        const_iterator  end() const { return to_object(m_size); }
        const_iterator cend() const { return to_object(m_size); }

        // reverse iterator support
#if !defined(BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION) && !defined(BOOST_MSVC_STD_ITERATOR) && !defined(BOOST_NO_STD_ITERATOR_TRAITS)
        typedef std::reverse_iterator<iterator> reverse_iterator;
        typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
#elif defined(_MSC_VER) && (_MSC_VER == 1300) && defined(BOOST_DINKUMWARE_STDLIB) && (BOOST_DINKUMWARE_STDLIB == 310)
        // workaround for broken reverse_iterator in VC7
        typedef std::reverse_iterator<std::_Ptrit<value_type, difference_type, iterator,
                                      reference, iterator, reference> > reverse_iterator;
        typedef std::reverse_iterator<std::_Ptrit<value_type, difference_type, const_iterator,
                                      const_reference, iterator, reference> > const_reverse_iterator;
#elif defined(_RWSTD_NO_CLASS_PARTIAL_SPEC) 
        typedef std::reverse_iterator<iterator, std::random_access_iterator_tag, 
              value_type, reference, iterator, difference_type> reverse_iterator; 
        typedef std::reverse_iterator<const_iterator, std::random_access_iterator_tag,
              value_type, const_reference, const_iterator, difference_type> const_reverse_iterator;
#else
        // workaround for broken reverse_iterator implementations
        typedef std::reverse_iterator<iterator,T> reverse_iterator;
        typedef std::reverse_iterator<const_iterator,T> const_reverse_iterator;
#endif

        reverse_iterator rbegin() { return reverse_iterator(end()); }
        const_reverse_iterator rbegin() const {
            return const_reverse_iterator(end());
        }
        const_reverse_iterator crbegin() const {
            return const_reverse_iterator(end());
        }

        reverse_iterator rend() { return reverse_iterator(begin()); }
        const_reverse_iterator rend() const {
            return const_reverse_iterator(begin());
        }
        const_reverse_iterator crend() const {
            return const_reverse_iterator(begin());
        }

        StaticVector():m_size(0){}

        StaticVector(size_type n, const_reference value):
          m_size(n)
        {
          insert(begin(),n,value);
        }

        template<typename InputIterator>
        StaticVector(InputIterator first, InputIterator last):
          m_size(last-first)
        {
          capacitycheck(size());
          copy_impl(first,last,begin());
        }

        template<std::size_t SizeRHS>
        StaticVector(const StaticVector<T,SizeRHS>& rhs):
          m_size(rhs.size())
        {
          capacitycheck(rhs.size());
          copy_impl<StaticVector<T,SizeRHS>::iterator,T,N>(rhs.begin(),rhs.end(),begin());
        }

        StaticVector(const StaticVector<T,N>& rhs):
          m_size(rhs.size())
        {
          copy_impl(rhs.begin(),rhs.end(),begin());
        }

        ~StaticVector(){
          destroy_array(::boost::has_trivial_destructor<T>());
        }

        void push_back (const_reference x){
          capacitycheck(size()+1);
          new (to_object(size())) T(x);
          m_size++;
        }

        void unchecked_push_back (const_reference x){
        }

        void pop_back(){
          if(!empty()){
            m_size--;
            to_object(size())->~T();
          } else {
            BOOST_THROW_EXCEPTION( std::out_of_range("StaticVector<> pop called on empty container."));
          }
        }

        iterator insert(iterator pos, const_reference x){
          capacitycheck(size()+1);
          copy_backward_impl(pos,end(),end()+1);
          *pos = x;
          m_size++;
          return pos;
        }

        void insert(iterator pos, max_size_type n, const_reference x){
          capacitycheck(size()+n);
          copy_backward_impl(pos,end(),end()+n);
          std::fill(pos,pos+n,x);
          m_size+=n;
        }

        template <typename InputIterator>
        void insert(iterator pos, InputIterator first, InputIterator last){
          max_size_type n = last - first;
          capacitycheck(size()+n);
          copy_backward_impl(pos,end(),end()+n);
          copy_impl(first,last,pos);
        }

        iterator erase(iterator pos){
          rangecheck(pos-begin());
          pos->~T();
          copy_impl(pos+1,end(),pos);
          m_size--;
          return pos;
        }

        iterator erase(iterator first, iterator last){
          std::ptrdiff_t n = last-first;
	        if (n>0) {
      	    rangecheck(size()-n);
      	    for(iterator it = first; it!=last; it++){
      	      it->~T();
      	    }
      	    copy_impl(last,end(),first);
      	    m_size -= n;
      	  }
          return first;
        }

        void clear(){
          erase(begin(),end());
        }

        void resize(max_size_type n, const_reference t = T() ){
          capacitycheck(n);
          if(n - m_size > 0){
            std::fill_n(end(), n-m_size, t);
          } else {
            erase(begin()+n,end());
          }
	        m_size = n;
	      }

        void reserve(max_size_type n){
          capacitycheck(n);
        }

        // operator[]
        reference operator[](max_size_type i) 
        { 
            BOOST_ASSERT( i < N || i < size() && "StaticVector<>: out of range" );
            return *to_object(i);
        }
        
        const_reference operator[](max_size_type i) const 
        {     
            BOOST_ASSERT( i < N || i < size() && "StaticVector<>: out of range" );
            return *to_object(i); 
        }

        // at() with range check
        reference at(max_size_type i) { rangecheck(i); return *to_object(i); }
        const_reference at(max_size_type i) const { rangecheck(i); return *to_object(i); }
    
        // front() and back()
        reference front() 
        { 
            return begin(); 
        }
        
        const_reference front() const 
        {
            return begin();
        }
        
        reference back() 
        { 
            return *to_object(size()-1);
        }
        
        const_reference back() const 
        { 
            return *to_object(size()-1);
        }

        // capacity is constant, size varies
        inline max_size_type size() const { return m_size; }
        inline static max_size_type capacity() { return N; }
        bool empty() { return size() == 0; }
        bool full() { return size() >= capacity(); }
        static max_size_type max_size() { return N; }
        enum { static_size = N };

        // swap (note: linear complexity)
        void swap (StaticVector<T,N>& y) {
#if ((BOOST_VERSION / 100) % 1000) > 44
            for (size_type i = 0; i < N; ++i)
                boost::swap(*to_object(i),*y.to_object(i));
            boost::swap(m_size,y.m_size);
#else
            std::swap_ranges(begin(),end(),y.begin());
            std::swap(m_size,y.m_size);
#endif
        }

        // direct access to data (read-only)
        const_pointer data() const { return elems; }
        pointer data() { return elems; }

        // use array as C array (direct read/write access to data)
        pointer c_array() { return elems; }

        // assignment with type conversion
        template <typename T2>
        StaticVector<T,N>& operator= (const StaticVector<T2,N>& rhs) {
            copy_impl(rhs.begin(),rhs.end(), begin());
            m_size = rhs.size();
            return *this;
        }

        // assign one value to all elements
        void assign (const T& value) { fill ( value ); }    // A synonym for fill
        void fill   (const T& value)
        {
            assign_impl(begin(),end(),value);
        }

        // check range (may not be private because it is not static)
        void rangecheck (max_size_type i) {
            if (i >= size()) {
                std::out_of_range e("StaticVector<>: index out of range");
                BOOST_THROW_EXCEPTION(e);
            }
        }

private:
       // check capacity (may be private because it is static)
      static void capacitycheck (max_size_type i) {
           if (i > N) {
               std::out_of_range e("StaticVector<>: index out of capacity");
               BOOST_THROW_EXCEPTION(e);
           }
       }
       
       inline const_pointer to_object(size_type index) const {
           return reinterpret_cast<const_pointer>(elems+index);
       }
       
       inline pointer to_object(size_type index) {
           return reinterpret_cast<pointer>(elems+index);
       }
       
       // T has a trivial destructor, do nothing
       inline void destroy_array(const boost::true_type&) {}
       
       // T has a destructor, destroy each object 
       inline void destroy_array(const boost::false_type&) {
           for(iterator first = begin(); first != end(); ++first) {
              first->~T();
           }
       }
       
       template< class InputIterator>
       static void copy_impl( InputIterator begin, InputIterator end, iterator result, std::random_access_iterator_tag )
       {
           copy_rai( begin, end, result, boost::has_trivial_assign<T>() );
       }
       template< class InputIterator>
       static void copy_impl( InputIterator begin, InputIterator end, iterator result, boost::random_access_traversal_tag )
       {
           copy_rai( begin, end, result, boost::has_trivial_assign<T>() );
       }
       
       static void copy_rai( const_iterator  begin, const_iterator end, 
                             iterator result, const boost::true_type& )
       {
           std::memcpy( result, begin, sizeof(T) * std::distance(begin,end) );
       }
       
       template< class InputIterator, bool b >
       static void copy_rai( InputIterator begin, InputIterator end, 
                             iterator result, const boost::integral_constant<bool, b>& )
       {
           std::uninitialized_copy( begin, end, result );
       }        
       
       template< class InputIterator>
       static void copy_impl( InputIterator begin, InputIterator end, iterator result, boost::single_pass_traversal_tag )
       {
           std::uninitialized_copy( begin, end, result );
       }
       
       template< class InputIterator>
       static void copy_impl( InputIterator begin, InputIterator end, iterator result )
       {
           copy_impl( begin, end, result, typename boost::iterator_category<InputIterator>::type() );
       }
       
       template< class InputIterator>
       static void copy_backward_impl( InputIterator begin, InputIterator end, iterator result, std::random_access_iterator_tag )
       {
           copy_backward_rai( begin, end, result, boost::has_trivial_assign<T>() );
       }
       
       template< class InputIterator>
       static void copy_backward_impl( InputIterator begin, InputIterator end, iterator result, boost::random_access_traversal_tag )
       {
           copy_backward_rai( begin, end, result, boost::has_trivial_assign<T>() );
       }
       
       template< class InputIterator>
       static void copy_backward_rai( iterator begin, iterator end, 
                             iterator result, const boost::true_type& )
       {
           std::memmove( result, begin, sizeof(T) * std::distance(begin,end) );
       }
       
       template< class InputIterator, bool b >
       static void copy_backward_rai( InputIterator begin, InputIterator end, 
                             iterator result, const boost::integral_constant<bool, b>& )
       {
           detail::uninitialized_copy_backward( begin, end, result );
       }
       
       template< class InputIterator>
       static void copy_backward_impl( InputIterator begin, InputIterator end, iterator result, boost::single_pass_traversal_tag )
       {
           detail::uninitialized_copy_backward( begin, end, result );
       }
       
       template< class InputIterator>
       static void copy_backward_impl( InputIterator begin, InputIterator end, iterator result )
       {
           copy_backward_impl( begin, end, result, typename boost::iterator_category<InputIterator>::type() );
       }
       
       template< class InputIterator>
       static void assign_impl( InputIterator begin, InputIterator end, iterator  result )
       {
           assign_impl( begin, end, result, boost::has_trivial_assign<T>() );
       }
       
       template< class InputIterator>
       static void assign_impl( InputIterator begin, InputIterator end, iterator  result, const boost::true_type& )
       {
           std::memcpy( result, begin, sizeof(T) * std::distance(begin,end) ); 
       }
       
       template< class InputIterator>
       static void assign_impl( InputIterator begin, InputIterator end, iterator  result, const boost::false_type& )
       {
           for( ; begin != end; ++begin, ++result )
               *result = *begin;
       }
}; // class StaticVector

#if !defined(BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)
    template< class T >
    class StaticVector< T, 0 > {

      public:
        // type definitions
        typedef T                                              value_type;
        typedef T*                                             iterator;
        typedef const T*                                       const_iterator;
        typedef T&                                             reference;
        typedef const T&                                       const_reference;
        typedef typename boost::aligned_storage<               
                           sizeof(T),                          
                           boost::alignment_of<T>::value       
                         >::type                               aligned_storage;
        typedef typename boost::uint_value_t<0>::least         size_type;
        typedef std::size_t                                    max_size_type;
        typedef std::ptrdiff_t                                 difference_type;

        // iterator support
        iterator        begin()       { return       iterator( reinterpret_cast<       T * >( this ) ); }
        const_iterator  begin() const { return const_iterator( reinterpret_cast< const T * >( this ) ); }
        const_iterator cbegin() const { return const_iterator( reinterpret_cast< const T * >( this ) ); }

        iterator        end()       { return  begin(); }
        const_iterator  end() const { return  begin(); }
        const_iterator cend() const { return cbegin(); }

        // reverse iterator support
#if !defined(BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION) && !defined(BOOST_MSVC_STD_ITERATOR) && !defined(BOOST_NO_STD_ITERATOR_TRAITS)
        typedef std::reverse_iterator<iterator> reverse_iterator;
        typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
#elif defined(_MSC_VER) && (_MSC_VER == 1300) && defined(BOOST_DINKUMWARE_STDLIB) && (BOOST_DINKUMWARE_STDLIB == 310)
        // workaround for broken reverse_iterator in VC7
        typedef std::reverse_iterator<std::_Ptrit<value_type, difference_type, iterator,
                                      reference, iterator, reference> > reverse_iterator;
        typedef std::reverse_iterator<std::_Ptrit<value_type, difference_type, const_iterator,
                                      const_reference, iterator, reference> > const_reverse_iterator;
#elif defined(_RWSTD_NO_CLASS_PARTIAL_SPEC) 
        typedef std::reverse_iterator<iterator, std::random_access_iterator_tag, 
              value_type, reference, iterator, difference_type> reverse_iterator; 
        typedef std::reverse_iterator<const_iterator, std::random_access_iterator_tag,
              value_type, const_reference, const_iterator, difference_type> const_reverse_iterator;
#else
        // workaround for broken reverse_iterator implementations
        typedef std::reverse_iterator<iterator,T> reverse_iterator;
        typedef std::reverse_iterator<const_iterator,T> const_reverse_iterator;
#endif

        reverse_iterator rbegin() { return reverse_iterator(end()); }
        const_reverse_iterator rbegin() const {
            return const_reverse_iterator(end());
        }
        const_reverse_iterator crbegin() const {
            return const_reverse_iterator(end());
        }

        reverse_iterator rend() { return reverse_iterator(begin()); }
        const_reverse_iterator rend() const {
            return const_reverse_iterator(begin());
        }
        const_reverse_iterator crend() const {
            return const_reverse_iterator(begin());
        }

        void push_back (const_reference x){
          failed_rangecheck();
        }

        void pop_back(){
          failed_rangecheck();
        }

        iterator insert(iterator pos, const_reference x){
          return failed_rangecheck();
        }

        void insert(iterator pos, max_size_type n, const_reference x){
          failed_rangecheck();
        }

        template <typename InputIterator>
        void insert(iterator pos, InputIterator first, InputIterator last){
          failed_rangecheck();
        }

        iterator erase(iterator pos){
          return failed_rangecheck();
        }

        iterator erase(iterator first, iterator last){
          return failed_rangecheck();
        }

        void clear(){
        }

        void resize(max_size_type n, const_reference t = T() ){
          failed_rangecheck();
        }

        void reserve(size_type n){
          failed_rangecheck();
        }

        // operator[]
        reference operator[](max_size_type /*i*/)
        {
            return failed_rangecheck();
        }

        const_reference operator[](max_size_type /*i*/) const
        {
            return failed_rangecheck();
        }

        // at() with range check
        reference at(max_size_type /*i*/)               {   return failed_rangecheck(); }
        const_reference at(max_size_type /*i*/) const   {   return failed_rangecheck(); }

        // front() and back()
        reference front()
        {
            return failed_rangecheck();
        }

        const_reference front() const
        {
            return failed_rangecheck();
        }

        reference back()
        {
            return failed_rangecheck();
        }

        const_reference back() const
        {
            return failed_rangecheck();
        }

        // size is constant
        inline static max_size_type size() { return 0; }
        inline static bool empty() { return true; }
        inline static bool full() { return true; }
        inline static max_size_type max_size() { return 0; }
        enum { static_size = 0 };

        void swap (StaticVector<T,0>& /*y*/) {
        }

        // direct access to data (read-only)
        const T* data() const { return 0; }
        T* data() { return 0; }

        // use array as C array (direct read/write access to data)
        T* c_array() { return 0; }

        // assignment with type conversion
        template <typename T2>
        StaticVector<T,0>& operator= (const StaticVector<T2,0>& ) {
            return *this;
        }

        // assign one value to all elements
        void assign (const T& value) { fill ( value ); }
        void fill   (const T& ) {}
        
        // check range (may be private because it is static)
        static reference failed_rangecheck () {
                std::out_of_range e("attempt to access element of an empty StaticVector");
                BOOST_THROW_EXCEPTION(e);
#if defined(BOOST_NO_EXCEPTIONS) || !defined(BOOST_MSVC)
                //
                // We need to return something here to keep
                // some compilers happy: however we will never
                // actually get here....
                //
                static T placeholder;
                return placeholder;
#endif
            }
    };
#endif

    // comparisons
    template<class T, std::size_t N>
    bool operator== (const StaticVector<T,N>& x, const StaticVector<T,N>& y) {
        return std::equal(x.begin(), x.end(), y.begin());
    }
    template<class T, std::size_t N>
    bool operator< (const StaticVector<T,N>& x, const StaticVector<T,N>& y) {
        return std::lexicographical_compare(x.begin(),x.end(),y.begin(),y.end());
    }
    template<class T, std::size_t N>
    bool operator!= (const StaticVector<T,N>& x, const StaticVector<T,N>& y) {
        return !(x==y);
    }
    template<class T, std::size_t N>
    bool operator> (const StaticVector<T,N>& x, const StaticVector<T,N>& y) {
        return y<x;
    }
    template<class T, std::size_t N>
    bool operator<= (const StaticVector<T,N>& x, const StaticVector<T,N>& y) {
        return !(y<x);
    }
    template<class T, std::size_t N>
    bool operator>= (const StaticVector<T,N>& x, const StaticVector<T,N>& y) {
        return !(x<y);
    }

    // global swap()
    template<class T, std::size_t N>
    inline void swap (StaticVector<T,N>& x, StaticVector<T,N>& y) {
        x.swap(y);
    }

#if defined(__SUNPRO_CC)
//  Trac ticket #4757; the Sun Solaris compiler can't handle
//  syntax like 'T(&get_c_array(boost::StaticVector<T,N>& arg))[N]'
//  
//  We can't just use this for all compilers, because the 
//      borland compilers can't handle this form. 
    namespace detail {
       template <typename T, std::size_t N> struct c_array
       {
           typedef T type[N];
       };
    }
    
   // Specific for boost::StaticVector: simply returns its elems data member.
   template <typename T, std::size_t N>
   typename detail::c_array<T,N>::type& get_c_array(boost::StaticVector<T,N>& arg)
   {
       return arg.elems;
   }

   // Specific for boost::StaticVector: simply returns its elems data member.
   template <typename T, std::size_t N>
   typename const detail::c_array<T,N>::type& get_c_array(const boost::StaticVector<T,N>& arg)
   {
       return arg.elems;
   }
#else
// Specific for boost::StaticVector: simply returns its elems data member.
    template <typename T, std::size_t N>
    T(&get_c_array(boost::StaticVector<T,N>& arg))[N]
    {
        return arg.elems;
    }
    
    // Const version.
    template <typename T, std::size_t N>
    const T(&get_c_array(const boost::StaticVector<T,N>& arg))[N]
    {
        return arg.elems;
    }
#endif
    
#if 0
    // Overload for std::array, assuming that std::array will have
    // explicit conversion functions as discussed at the WG21 meeting
    // in Summit, March 2009.
    template <typename T, std::size_t N>
    T(&get_c_array(std::array<T,N>& arg))[N]
    {
        return static_cast<T(&)[N]>(arg);
    }
    
    // Const version.
    template <typename T, std::size_t N>
    const T(&get_c_array(const std::array<T,N>& arg))[N]
    {
        return static_cast<T(&)[N]>(arg);
    }
#endif



namespace detail{
        template< typename InputIterator, typename OutputIterator >
        static void uninitialized_copy_backward(InputIterator begin, InputIterator end, OutputIterator result){
            while (end!=begin) new (static_cast<void*>(&*(--result))) typename iterator_traits<InputIterator>::value_type(typename iterator_traits<InputIterator>::value_type(*(--end)));
            return result;
        }
} // namespace detail 

} // namespace boost


#if BOOST_WORKAROUND(BOOST_MSVC, >= 1400)  
# pragma warning(pop)  
#endif 

#endif /*BOOST_STATIC_VECTOR_HPP*/
