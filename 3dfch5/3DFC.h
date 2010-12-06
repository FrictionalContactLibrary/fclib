/** @file 

    This API defines structure for 3D frictional contact problems.  
    These take the form of mixed complementarity problems or affine
    variational inequalities etc. 
 */


/// as often the case, the type for real numbers might have to be
/// parameterized.  Work on GPGPU for instance might have to be performed in
/// single precition
typedef double real;

/// 
///  The friction coefficients have to be defined using a class because one
///  might actually include both static and kinetic coefficients, as well
///  as directional coefficients for orthotropic frictional models, and
///  even perhaps a velocity dependent friction coefficient. 
///
struct Friction { 
  /* .... some data here.  Simplest case: a scalar
   but there should be support for the following: 
   a-) both kinetic and static coefficients
   b-) velocity dependent static coefficient
   c-) orthotropic
   d-) my suggestion at this point is to have a class hierarchy here.
  */
};

///
/// A basic storage for sparse matrices
//
struct SparseMatrix { 
  ///  ... routine definition of sparse matrix
  int m_nnz;                    // number of nonzero elements
  int m_rows;                  // number of rows
  int m_cols;                  // number of columns
  
  /// This class will allocate this data on loading. 
  int * m_ptr;                  // pointers to start of each column
  int * m_idx;                 // row index of each non-zero value
  real * data;                  // row index of each non-zero value

  
  /// release the memory.  This is not done automatically in the destructor
  /// since the user may want to use his/her own sparse matrix data
  /// structure and use the arrays allocated during the loading stage.  
  /// One can inherit from this class and automate the destructor. 
  virtual int release() { 
    m_nnz = m_rows = m_cols = 0; 
    if ( m_ptr ) { delete [] m_ptr;  m_ptr = 0; }
    if ( m_idx ) { delete [] m_idx;  m_idx = 0; }
    if ( m_data ) { delete [] m_data;  m_data = 0; }
}
  /// HDF5 components

  /// store all the data defined above with suitable strings
  /// it is extremely important to have a virtual here since inheritance 
  /// for this class is probably the way to support other formats such as
  /// blocks etc.  These formats can then have a special dump function to
  /// unravel blocks etc.  In addition, this is where the conversion from
  /// row to column major would happen
  virtual int dumpH5(std::string& name, H5::Group &g);

  /// H5 constructor:  find matrix with given name in given H5 group;
  /// Recursively find the member of the sparse matrix named.
  SparseMatrix(const H5::Group& g, const std::string &name  ) ;
  SparseMatrix() { 
    m_nnz = m_rows = m_cols = 0; 
    m_ptr = m_idx = 0; 
    m_data = 0; 
  }
  ~SparseMatrix(){}
};



/// Constraint equations not related to friction must be allowed to have
/// upper and lower bounds as well.  Equality constraints have upper bounds
/// at Inf and lower bonds at -Inf
struct BoxBounds { 
  std::valarray<real>  upper;
  std::valarray<real>  lower;
  virtual int dumpH5(std::string& name, H5::Group &g);
  BoxBound(const H5::Group& g, const std::string &name  ) ;
};



///
/// A contact constraint is a sparse submatrix which consists of 3 columns
/// for each contact.  Every 3rd column is a normal constraint, the next 2
/// are the tangential components. 
///
/// The first one is for the normal constraint, the other two are for
/// tangential ones. 
/// 
/// The local contact velocity is the product of the contact matrix times
/// the body velocities.
/// 
///
struct ContactConstraints {
  std::vector<Friction>     friction;
  SparseMatrix W;
  virtual int dumpH5(std::string& name, H5::Group &g);
  ContactConstraints(const H5::Group& g, const std::string &name  ) ;
};

/// 
///  This is the most general definition in Vincent's 
///
struct GM3DFC { 
  SparseMatrix               M; // mass matrix
  SparseMatrix               G; // Jacobian for constraints other than contacts
  BoxBounds                  B; // bounds for Lagrange multipliers for G constraints
  std::valarray<real>        b; // Vectors for constraints:  Gv + b = 0
  std::valarray<real>        f; // external forces
  std::vector<ContactConstraints>  C; // list of contact constraints, which
                                     // include friction coefficients as well
                                     // as normal and tangent Jacobians
  std::valarray<real>        v;      // the velocities

  /// can include guesses or known answers here
  std::valarray<real>        lambda; // multipliers for general constraints
  std::valarray<real>        r;      // multipliers contacts.

  virtual int dumpH5(std::string& name, H5::Group &g);
  virtual GM3DFC(const H5::Group& g, const std::string &name  ) ;
};


