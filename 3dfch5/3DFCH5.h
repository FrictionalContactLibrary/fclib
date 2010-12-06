/**
   Author: Claude Lacoursiere
   August 3 2010
 */
#ifndef M3DCLIB_H5
#define M3DCLIB_H5


#include <string>
#include <valarray>
#include <string.h>
#include <iostream>

#include <H5Cpp.h>
#include <H5Tpublic.h>

namespace 3dcflib { 

  namespace h5 {

    extern const std::string format;
    extern const std::string problem_type;
    extern const std::string problem_mlcp;
    extern const std::string problem_3DFC;
    extern const std::string problem_GM3DFC;
    extern const std::string problem_G3DFC;
    extern const std::string sparse_matrix;
    extern const std::string scalar;
    extern const std::string value;



    /// Iterators traverse an H5 file.  This particular library relies on
    /// finding named data groups which are parts of structs making this
    /// definition useful.
    struct IteratorData { 
      IteratorData() : m_name() {}
      std::string m_name;
    }; 

    /** A simple wrapper class for the annoying hsize_t dims[2] arrays which
        are used all the time. */
    class h2s  { 
      public: 
      hsize_t m[2]; 
      h2s(hsize_t a, hsize_t b) { m[0] =a; m[1]=b;}
      h2s(hsize_t a) { m[0] =a; m[1]=1;}
      h2s() { m[0] =0; m[1]=0;}
      #if !defined(__APPLE__) 
      #ifndef WIN32
      //\TODO Fix mac & windows compiling
      hsize_t & operator[](size_t i) { return m[i] ; }
      hsize_t operator[](size_t i) const  { return m[i] ; }
      #endif
      #endif
      operator const hsize_t * () const { return m ; }
      operator hsize_t * () { return m ; }
    };

    /** Dump an array of integers to an open HDF5 group. */ 
    void dump_array(const int *a, size_t n, const std::string& name, H5::Group & g);

    /** Dump an array of doubles to an open HDF5 group. */ 
    void dump_array(const double *a, size_t n, const std::string& name, H5::Group & g);

    /** Dump a text string to an open HDF5 group. */ 
    void dump_string(const std::string & s, const std::string &name, H5::Group & g);

    /** Dump an integer to an open HDF5 group. */ 
    void dump_scalar(int a, const std::string &name, H5::Group & g);

    /** Dump a double precision scalar to an open HDF5 group. */ 
    void dump_scalar(double a, const std::string &name, H5::Group & g);




    /** Add a scalar attribute of the given name to an open HDF5 group and set value to 1. */
    void set_scalar_attribute(const std::string &name, int val, H5::Group & g);

    /**  A utility that just checks for the existence of an attribute with given name */
    bool check_attr(const H5::Group &g, const std::string & name);

    /** Read the string dataset of given name into the std::string buff.
        Return 0 if not found, 1 for success.  */
    int get_string(std::string & buff,  const H5::Group& g, const std::string &name  );
    /** Read the scalar integer dataset of given name and return it.  Return
        0 if not found.  */ 

    int get_integer(const H5::Group& g, const std::string &name  );
    /** Read the integer array dataset of given name and store it in a heap
        allocated std::valarray<int>.  Returns pointer to that valarray or 0 if
        the dataset is not found (or is not of int type) */
    std::valarray<int> * get_integer_array(const H5::Group& g, const std::string &name );


    /// ---> Templates could have been used here.  Also, float is not used
    /// ---> at all. 
    ///
    ///  The _oct suffix indicates that appropriate strings are dumped in
    ///  the H5 file so Octave can read everything correctly.  This is not
    ///  strictly necessary. 

    /** Dump an array of integers to an open HDF5 group, and format the
        result so it can be read by the Octave loader. */ 
    void dump_array_oct(const int *a, size_t n, const std::string& name, H5::Group & g);

    /** Dump an array of double to an open HDF5 group, and format the
        result so it can be read by the Octave loader. */ 
    void dump_array_oct(const double *a, size_t n, const std::string& name, H5::Group & g);

    /** Dump a text string to an open HDF5 group. */ 
    void dump_string_oct(const std::string & s, const std::string &name, H5::Group & g);
    /** Dump an integer to an open HDF5 group. */ 
    void dump_scalar_oct(int a, const std::string &name, H5::Group & g);
    /** Dump a double precision scalar to an open HDF5 group. */ 
    void dump_scalar_oct(double a, const std::string &name, H5::Group & g);


    /**
       Below are several different ways to load arrays from the H5 file.
       A choice must be made.
     */


    /** Read the integer array dataset of given name and store it in the
        given std::valarray<int>.  This uses v.resize(n) internally. 
        Returns 0 for failure, and 1 for success. */
    int  get_array_oct(std::valarray<int> & v, const H5::Group& g, const std::string &name );
    int  get_array_oct(Vector<int> & v, const H5::Group& g, const std::string &name );

    int  get_array(std::valarray<int> & v, const H5::Group& g, const std::string &name );
    int  get_array(Vector<int> & v, const H5::Group& g, const std::string &name );
    /** Read the integer array dataset of given name and store it in a heap
        allocated std::valarray<double>.  Returns pointer to that valarray or 0 if
        the dataset is not found (or is not of double type) */
    std::valarray<double> * get_double_array(const H5::Group& g, const std::string &name );
    /** Read the integer array dataset of given name and store it in the
        given std::valarray<int>.  This uses v.resize(n) internally. 
        Returns 0 for failure, and 1 for success. */
    
    int  get_array(std::valarray<double> & v, const H5::Group& g, const std::string &name );
    
    int  get_array(Vector<double> & v, const H5::Group& g, const std::string &name );
    

    /// other versions can include "Fortran"-like pass by reference 
    /// semantic. 
    int  get_array(double *& v, int &l,  const H5::Group& g, const std::string &name );


    H5::H5File * append_or_create(const std::string & filename);
    H5::Group * append_problem(H5::H5File *file, const std::string & name = "Problem");

// HAVE_HDF5
#endif

  } /* namespace H5 */
} /* namespace 3dcflib */

#ifdef HAVE_HDF5
extern "C" { 
  herr_t list_nodes (hid_t group_id, const char *  member_name, void *operator_data);
}
#endif


// namespace
#endif

