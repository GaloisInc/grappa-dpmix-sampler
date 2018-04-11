
#include "datatypes.h"

double compute_pdf (struct distribution *dist, union value *data,
                    union value vs[], unsigned int value_pos) {
  double ret_value = 0;
  unsigned int i;
  union value *sub_data;
  struct distribution *sub_dist;
  struct var_length_array *var_array;

  switch (dist->tp) {
  case cont_dist_type:
    vs[value_pos].double_value = data->double_value;
    return dist->dist.atomic.pdf (vs);

  case discr_dist_type:
    vs[value_pos].int_value = data->int_value;
    return dist->dist.atomic.pdf (vs);

  case tuple_dist_type:
    sub_data = data;
    for (i = 0; i < dist->dist.tuple.num_dists; ++i) {
      sub_dist = dist->dist.tuple.elem_dists[i];
      ret_value += compute_pdf (sub_dist, sub_data, vs, value_pos + i);
      sub_data += sub_dist->size;
    }
    vs[value_pos].fixed_array_value = data;
    return ret_value;

  case fixed_array_dist_type:
    sub_data = data;
    for (i = 0; i < dist->dist.fixed_array.length; ++i) {
      sub_dist = dist->dist.fixed_array.elem_dist;
      ret_value += compute_pdf (sub_dist, sub_data, vs, value_pos + i);
      sub_data += sub_dist->size;
    }
    vs[value_pos].fixed_array_value = data;
    return ret_value;

  case var_array_dist_type:
    var_array = data->var_array_value;
    sub_dist = dist->dist.var_array.elem_dist;
    for (i = 0; i < var_array->length; ++i) {
      ret_value +=
        compute_pdf (sub_dist, &(var_array->elems[i]), vs, value_pos);
    }
    vs[value_pos].var_array_value = var_array;
    return ret_value;
  }
}

unsigned int max (unsigned int x, unsigned int y) {
  if (x >= y) { return x; } else { return y; }
}

double compute_dpmix_pdf (struct dpmix_distribution *dist,
                          union value *params, union value *data) {
  unsigned int max_width = max (dist->num_param_doubles, dist->data_width + 1);
  union value vs[dist->num_param_doubles + dist->data_width];
  double pdf1 = compute_pdf (dist->param_dist, params, vs, 0);
  double pdf2 = compute_pdf (dist->data_dist, data, vs, 1);
  return pdf1 + pdf2;
}
