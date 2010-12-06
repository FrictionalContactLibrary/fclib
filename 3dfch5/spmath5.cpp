
  bool SparseMatrix::dumpH5(void* group, const std::string& name ) const
  {

    bool ret = false;
    H5::Group *gp =   static_cast<H5::Group *>(group); 

    if ( gp ) 
    {
      using namespace 3dfclib::h5;

      // create group with the name of the matrix
      H5::Group mg = gp->createGroup(name);

      // tag the format being used
      dump_string(oct_sparse_matrix, oct_type, mg); 

      H5::Group vg = mg.createGroup(oct_value);

      dump_scalar((int)m_nnz, "nz", vg);
      dump_scalar((int)m_rows, "nr", vg);
      dump_scalar((int)m_cols, "nc", vg);

      /// note that in row major order, the transpose of the matrix gets loaded in 
      /// octave or matlab
      dump_array(m_pointers, m_cols+1, "cidx", vg);
      dump_array(m_idx,  m_nnz, "ridx", vg);
      dump_array(m_data, values.size(), "data", vg);

      vg.close();
      mg.close();
      ret = true;
    }
    return ret;
  }
