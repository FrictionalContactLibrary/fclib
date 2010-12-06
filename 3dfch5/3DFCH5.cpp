/*
Copyright 2008. Algoryx Simulation AB. 

All AgX source code, intellectual property, documentation, sample code, 
tutorials, scene files and technical white papers, are copyrighted, proprietary 
and confidential material of Algoryx Simulation AB. You may not download, read, 
store, distribute, publish, copy or otherwise disseminate, use or expose this 
material without having a written signed agreement with Algoryx Simulation AB.

Algoryx Simulation AB disclaims all responsibilities for loss or damage caused
from using this software, unless otherwise stated in written agreements with
Algoryx Simulation AB. 
*/

#ifdef HAVE_HDF5
#include <3Dfclib.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>

using namespace H5;
using namespace 3dfclib::h5;

namespace 3dfclib { 
  namespace h5 { 

    bool check_attr(const H5::Group &g, const std::string & name){
      bool ret = false; 
      try { 
        Exception::dontPrint();
        Attribute a = g.openAttribute(name);
        a.close();
        ret = true;
      }
      catch(H5::AttributeIException error) { 
      }
      return ret;
    }


    void dump_array(const int *a, size_t n, const std::string& name, H5::Group& g){
      DataSpace dataspace( 2, h2s(n));
      DSetCreatPropList prop; 
      //prop.setChunk(2, h2s(n)); 
      //prop.setDeflate(9);
      IntType datatype( PredType::NATIVE_INT );
      DataSet dataset = g.createDataSet( name, datatype, dataspace, prop );
      dataset.write( a, PredType::NATIVE_INT );
      dataset.close();
    }

    void dump_array(const double *a, size_t n, const std::string& name, H5::Group& g){
      DataSpace dataspace( 2, h2s(n));
      DSetCreatPropList prop; 
      //prop.setChunk(2, h2s(n)); 
      //prop.setDeflate(9);
      FloatType datatype( PredType::NATIVE_DOUBLE);
      DataSet dataset = g.createDataSet( name, datatype, dataspace, prop );
      dataset.write( a, PredType::NATIVE_DOUBLE );
      dataset.close();
    }

    void dump_string(const std::string &s, const std::string &name, H5::Group& g){
      DataSpace dataspace( 0, h2s());
      StrType datatype(PredType::C_S1, s.length()+1);
      DataSet dataset = g.createDataSet( name, datatype, dataspace );
      dataset.write( s, datatype);
      dataset.close();
    }

    void dump_scalar(int a, const std::string &name, H5::Group& g){
      DataSpace dataspace(H5S_SCALAR);
      IntType datatype(PredType::NATIVE_INT);
      DataSet dataset = g.createDataSet( name, datatype, dataspace );
      dataset.write( &a, datatype);
      dataset.close();
    }

    void dump_scalar(double a, const std::string &name, H5::Group& g){
      DataSpace dataspace(H5S_SCALAR);
      FloatType datatype(PredType::NATIVE_DOUBLE);
      DataSet dataset = g.createDataSet( name, datatype, dataspace );
      dataset.write( &a, datatype);
      dataset.close();
    }

    void set_scalar_attribute(const std::string &name, int val, H5::Group& g){
      DataSpace dataspace(H5S_SCALAR);
      IntType datatype(PredType::NATIVE_INT);
      Attribute attr = g.createAttribute(name, datatype, dataspace);
      int v = val;
      attr.write(datatype, &v);
    }


    int get_string(std::string & buff,  const H5::Group& g, const std::string &name  ) { 
      int ret = 0; 
      try { 
        Exception::dontPrint();
        DataSet t  = g.openDataSet(name);
        try { 
          //Exception::dontPrint();
          buff.resize(t.getStorageSize());
          t.read(buff, t.getStrType());
          t.close(); 
          ret = 1; 
        } catch ( H5::DataSetIException error ) { 
        } 
      }
      catch (H5::GroupIException error ) { 
      }
      return ret;
    }

    int get_integer(const H5::Group& g, const std::string &name  ) { 
      int ret = 0; 
      try { 
        Exception::dontPrint();
        DataSet t  = g.openDataSet(name);
        try { 
          Exception::dontPrint();
          t.read(&ret, t.getIntType());
          t.close(); 
        } catch ( H5::DataSetIException error ) { 
        } 
      }
      catch (H5::GroupIException error ) { 
      }
      return ret;
    }

    std::valarray<int> * get_integer_array(const H5::Group& g, const std::string &name  ) { 
      std::valarray<int> *ret = 0;
      try { 
        Exception::dontPrint();
        DataSet t  = g.openDataSet(name);
        try { 
          Exception::dontPrint();
          DataSpace ds = t.getSpace(); 
          h2s dims;
          ds.getSimpleExtentDims(dims);
          ret = new std::valarray<int>(dims[0]);
          t.read(&((*ret)[0]), t.getIntType());
          t.close(); 
        } catch ( H5::DataSetIException error ) { 
        } 
      }
      catch (H5::GroupIException error ) { 
      }
      return ret;
    }

    std::valarray<double> * get_double_array(const H5::Group& g, const std::string &name  ) { 
      std::valarray<double> *ret = 0;
      try { 
        Exception::dontPrint();
        DataSet t  = g.openDataSet(name);
        try { 
          Exception::dontPrint();
          DataSpace ds = t.getSpace(); 
          h2s dims;
          ds.getSimpleExtentDims(dims);
          ret = new std::valarray<double>(dims[0]);
          t.read(&((*ret)[0]), t.getFloatType());
          t.close(); 
        } catch ( H5::DataSetIException error ) { 
        } 
      }
      catch (H5::GroupIException error ) { 
      }
      return ret;
    }

    int  get_array(std::valarray<double>& v, const H5::Group& g, const std::string &name  ) { 
      int ret = 0;
      try { 
        Exception::dontPrint();
        DataSet t  = g.openDataSet(name);
        try { 
          Exception::dontPrint();
          DataSpace ds = t.getSpace(); 
          h2s dims;
          ds.getSimpleExtentDims(dims);
          v.resize(dims[0]);
          t.read(&(v[0]), t.getFloatType());
          t.close(); 
          ret = 1; 
        } catch ( H5::DataSetIException error ) { 
        } 
      }
      catch (H5::GroupIException error ) { 
      }
      return ret;
    }


    int  get_array(Vector<double>& v, const H5::Group& g, const std::string &name  ) { 
      int ret = 0;
      try { 
        Exception::dontPrint();
        DataSet t  = g.openDataSet(name);
        try { 
          Exception::dontPrint();
          DataSpace ds = t.getSpace(); 
          h2s dims;
          ds.getSimpleExtentDims(dims);
          v.resize(dims[0]);
          t.read(&(v[0]), t.getFloatType());
          t.close(); 
          ret = 1; 
        } catch ( H5::DataSetIException error ) { 
        } 
      }
      catch (H5::GroupIException error ) { 
      }
      return ret;
    }

    int  get_array(std::valarray<int>& v, const H5::Group& g, const std::string &name  ) { 
      int ret = 0;
      try { 
        Exception::dontPrint();
        DataSet t  = g.openDataSet(name);
        try { 
          Exception::dontPrint();
          DataSpace ds = t.getSpace(); 
          h2s dims;
          ds.getSimpleExtentDims(dims);
          v.resize(dims[0]);
          t.read(&(v[0]), t.getIntType());
          ret = 1; 
          t.close(); 
        } catch ( H5::DataSetIException error ) { 
        } 
      }
      catch (H5::GroupIException error ) { 
      }
      return ret;
    }

    


    int  get_array(Vector<int>& v, const H5::Group& g, const std::string &name  ) { 
      int ret = 0;
      try { 
        Exception::dontPrint();
        DataSet t  = g.openDataSet(name);
        try { 
          Exception::dontPrint();
          DataSpace ds = t.getSpace(); 
          h2s dims;
          ds.getSimpleExtentDims(dims);
          v.resize(dims[0]);
          t.read(&(v[0]), t.getIntType());
          ret = 1; 
          t.close(); 
        } catch ( H5::DataSetIException error ) { 
        } 
      }
      catch (H5::GroupIException error ) { 
      }
      return ret;
    }



    /** Open an existing H5File to append or create it from scratch if it
    does not exist.  It is the caller's responsibility to close that file
    when writing is done. */
    H5::H5File * append_or_create(const std::string & filename) { 

      H5File *file = 0; 
      try { 
        Exception::dontPrint();
        file = new H5File(filename, H5F_ACC_RDWR);
      } catch( FileIException error ) { 
        //error.printError();
        file = new H5File(filename, H5F_ACC_TRUNC);
      }
      return file; 
    }
    
    /** Open a group with a unique name in an existing H5File.  The groups
        are labelled sequentially with an integer.  It is the caller's
        responsibility to close that group when done. */
    H5::Group * append_problem(H5File *file, const std::string & name ) { 
      const int problem_width = 6; 
      H5::Group  root = file->openGroup("/");
      int n_groups = root.getNumObjs();
			std::ostringstream buffer; 
      buffer << name << std::setw(agx_problem_width) << std::setfill('0') << ++n_groups; 
      std::string w = buffer.str();
      H5::Group g = root.createGroup(buffer.str());
      return new H5::Group(g);
    }

  }  /* namespace H5 */
} /* namespace 3dfclib */



#endif
