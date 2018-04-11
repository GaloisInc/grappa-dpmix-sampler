
/* A "value" is either an int, a double, or a pointer */
union value {
  int int_value;
  double double_value;
  union value *fixed_array_value;
  struct var_length_array *var_array_value;
};

/* A variable-length array of data elements */
struct var_length_array {
  unsigned int length;
  union value elems[];
};

/* An "atomic" (continuous or discrete) distribution */
struct atomic_dist {
  /* Return the log PDF for the current values that this PDF can "see"
     (including the one that was sampled from the current distribution) */
  double (*pdf)(union value vs[]);

  /* Compute the gradient of the log PDF for the current values, and accumulate
     it into the grad variable by adding in the computed gradient values to what
     is already there */
  void (*pdf_grad)(union value vs[], double grad[]);
};

struct tuple_dist {
  unsigned int num_dists;
  struct distribution *elem_dists[];
};

struct fixed_length_array_dist {
  unsigned int length;
  struct distribution *elem_dist;
};

struct var_length_array_dist {
  struct distribution *elem_dist;
};

enum dist_type {
  cont_dist_type,
  discr_dist_type,
  tuple_dist_type,
  fixed_array_dist_type,
  var_array_dist_type
};

struct distribution {

  /* The tag that indicates what type of distribution this is */
  enum dist_type tp;

  /* The size says how many values (not bytes) are taken up by this type */
  unsigned int size;

  /* The actual distribution itself */
  union {
    struct atomic_dist atomic;
    struct tuple_dist tuple;
    struct fixed_length_array_dist fixed_array;
    struct var_length_array_dist var_array;
  } dist;
};

struct dpmix_distribution {
  unsigned int num_param_doubles;
  struct distribution *param_dist;

  unsigned int data_width;
  struct distribution *data_dist;
};
