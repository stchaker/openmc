#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR=${1:-.}
echo "Applying xtensor include replacements in: ${ROOT_DIR}"

declare -A REPL=(

  # ───────────── ROOT ─────────────
  ['#include "xtensor/xtensor.hpp"']='#include "xtensor.hpp"'

  # ───────────── containers ─────────────
  ['#include "xtensor/xarray.hpp"']='#include "xtensor/containers/xarray.hpp"'
  ['#include "xtensor/xadapt.hpp"']='#include "xtensor/containers/xadapt.hpp"'
  ['#include "xtensor/xbuffer_adaptor.hpp"']='#include "xtensor/containers/xbuffer_adaptor.hpp"'
  ['#include "xtensor/xcontainer.hpp"']='#include "xtensor/containers/xcontainer.hpp"'
  ['#include "xtensor/xfixed.hpp"']='#include "xtensor/containers/xfixed.hpp"'
  ['#include "xtensor/xscalar.hpp"']='#include "xtensor/containers/xscalar.hpp"'
  ['#include "xtensor/xstorage.hpp"']='#include "xtensor/containers/xstorage.hpp"'

  # ───────────── chunk ─────────────
  ['#include "xtensor/xchunked_array.hpp"']='#include "xtensor/chunk/xchunked_array.hpp"'
  ['#include "xtensor/xchunked_assign.hpp"']='#include "xtensor/chunk/xchunked_assign.hpp"'
  ['#include "xtensor/xchunked_view.hpp"']='#include "xtensor/chunk/xchunked_view.hpp"'

  # ───────────── core ─────────────
  ['#include "xtensor/xaccessible.hpp"']='#include "xtensor/core/xaccessible.hpp"'
  ['#include "xtensor/xassign.hpp"']='#include "xtensor/core/xassign.hpp"'
  ['#include "xtensor/xeval.hpp"']='#include "xtensor/core/xeval.hpp"'
  ['#include "xtensor/xexpression.hpp"']='#include "xtensor/core/xexpression.hpp"'
  ['#include "xtensor/xexpression_traits.hpp"']='#include "xtensor/core/xexpression_traits.hpp"'
  ['#include "xtensor/xfunction.hpp"']='#include "xtensor/core/xfunction.hpp"'
  ['#include "xtensor/xiterable.hpp"']='#include "xtensor/core/xiterable.hpp"'
  ['#include "xtensor/xiterator.hpp"']='#include "xtensor/core/xiterator.hpp"'
  ['#include "xtensor/xlayout.hpp"']='#include "xtensor/core/xlayout.hpp"'
  ['#include "xtensor/xmath.hpp"']='#include "xtensor/core/xmath.hpp"'
  ['#include "xtensor/xmultiindex_iterator.hpp"']='#include "xtensor/core/xmultiindex_iterator.hpp"'
  ['#include "xtensor/xnoalias.hpp"']='#include "xtensor/core/xnoalias.hpp"'
  ['#include "xtensor/xoperation.hpp"']='#include "xtensor/core/xoperation.hpp"'
  ['#include "xtensor/xsemantic.hpp"']='#include "xtensor/core/xsemantic.hpp"'
  ['#include "xtensor/xshape.hpp"']='#include "xtensor/core/xshape.hpp"'
  ['#include "xtensor/xstrides.hpp"']='#include "xtensor/core/xstrides.hpp"'
  ['#include "xtensor/xtensor_config.hpp"']='#include "xtensor/core/xtensor_config.hpp"'
  ['#include "xtensor/xtensor_forward.hpp"']='#include "xtensor/core/xtensor_forward.hpp"'
  ['#include "xtensor/xvectorize.hpp"']='#include "xtensor/core/xvectorize.hpp"'

  # ───────────── generators ─────────────
  ['#include "xtensor/xbuilder.hpp"']='#include "xtensor/generators/xbuilder.hpp"'
  ['#include "xtensor/xgenerator.hpp"']='#include "xtensor/generators/xgenerator.hpp"'
  ['#include "xtensor/xrandom.hpp"']='#include "xtensor/generators/xrandom.hpp"'

  # ───────────── io ─────────────
  ['#include "xtensor/xcsv.hpp"']='#include "xtensor/io/xcsv.hpp"'
  ['#include "xtensor/xinfo.hpp"']='#include "xtensor/io/xinfo.hpp"'
  ['#include "xtensor/xio.hpp"']='#include "xtensor/io/xio.hpp"'
  ['#include "xtensor/xjson.hpp"']='#include "xtensor/io/xjson.hpp"'
  ['#include "xtensor/xmime.hpp"']='#include "xtensor/io/xmime.hpp"'
  ['#include "xtensor/xnpy.hpp"']='#include "xtensor/io/xnpy.hpp"'

  # ───────────── misc ─────────────
  ['#include "xtensor/xcomplex.hpp"']='#include "xtensor/misc/xcomplex.hpp"'
  ['#include "xtensor/xexpression_holder.hpp"']='#include "xtensor/misc/xexpression_holder.hpp"'
  ['#include "xtensor/xfft.hpp"']='#include "xtensor/misc/xfft.hpp"'
  ['#include "xtensor/xhistogram.hpp"']='#include "xtensor/misc/xhistogram.hpp"'
  ['#include "xtensor/xmanipulation.hpp"']='#include "xtensor/misc/xmanipulation.hpp"'
  ['#include "xtensor/xpad.hpp"']='#include "xtensor/misc/xpad.hpp"'
  ['#include "xtensor/xset_operation.hpp"']='#include "xtensor/misc/xset_operation.hpp"'
  ['#include "xtensor/xsort.hpp"']='#include "xtensor/misc/xsort.hpp"'
  ['#include "xtensor/xtl_concepts.hpp"']='#include "xtensor/misc/xtl_concepts.hpp"'

  # ───────────── optional ─────────────
  ['#include "xtensor/xoptional.hpp"']='#include "xtensor/optional/xoptional.hpp"'
  ['#include "xtensor/xoptional_assembly.hpp"']='#include "xtensor/optional/xoptional_assembly.hpp"'
  ['#include "xtensor/xoptional_assembly_base.hpp"']='#include "xtensor/optional/xoptional_assembly_base.hpp"'
  ['#include "xtensor/xoptional_assembly_storage.hpp"']='#include "xtensor/optional/xoptional_assembly_storage.hpp"'

  # ───────────── reducers ─────────────
  ['#include "xtensor/xaccumulator.hpp"']='#include "xtensor/reducers/xaccumulator.hpp"'
  ['#include "xtensor/xblockwise_reducer.hpp"']='#include "xtensor/reducers/xblockwise_reducer.hpp"'
  ['#include "xtensor/xblockwise_reducer_functors.hpp"']='#include "xtensor/reducers/xblockwise_reducer_functors.hpp"'
  ['#include "xtensor/xnorm.hpp"']='#include "xtensor/reducers/xnorm.hpp"'
  ['#include "xtensor/xreducer.hpp"']='#include "xtensor/reducers/xreducer.hpp"'

  # ───────────── utils ─────────────
  ['#include "xtensor/xexception.hpp"']='#include "xtensor/utils/xexception.hpp"'
  ['#include "xtensor/xtensor_simd.hpp"']='#include "xtensor/utils/xtensor_simd.hpp"'
  ['#include "xtensor/xutils.hpp"']='#include "xtensor/utils/xutils.hpp"'

  # ───────────── views ─────────────
  ['#include "xtensor/xaxis_iterator.hpp"']='#include "xtensor/views/xaxis_iterator.hpp"'
  ['#include "xtensor/xaxis_slice_iterator.hpp"']='#include "xtensor/views/xaxis_slice_iterator.hpp"'
  ['#include "xtensor/xbroadcast.hpp"']='#include "xtensor/views/xbroadcast.hpp"'
  ['#include "xtensor/xdynamic_view.hpp"']='#include "xtensor/views/xdynamic_view.hpp"'
  ['#include "xtensor/xfunctor_view.hpp"']='#include "xtensor/views/xfunctor_view.hpp"'
  ['#include "xtensor/xindex_view.hpp"']='#include "xtensor/views/xindex_view.hpp"'
  ['#include "xtensor/xmasked_view.hpp"']='#include "xtensor/views/xmasked_view.hpp"'
  ['#include "xtensor/xoffset_view.hpp"']='#include "xtensor/views/xoffset_view.hpp"'
  ['#include "xtensor/xrepeat.hpp"']='#include "xtensor/views/xrepeat.hpp"'
  ['#include "xtensor/xslice.hpp"']='#include "xtensor/views/xslice.hpp"'
  ['#include "xtensor/xstrided_view.hpp"']='#include "xtensor/views/xstrided_view.hpp"'
  ['#include "xtensor/xstrided_view_base.hpp"']='#include "xtensor/views/xstrided_view_base.hpp"'
  ['#include "xtensor/xview.hpp"']='#include "xtensor/views/xview.hpp"'
  ['#include "xtensor/xview_utils.hpp"']='#include "xtensor/views/xview_utils.hpp"'
)

# Replace includes across all C++ source and header files
find "$ROOT_DIR" -type f \( -name "*.cpp" -o -name "*.hpp" -o -name "*.h" -o -name "*.cc" -o -name "*.cxx" -o -name "*.tpp" \) |
while read -r file; do
  for old in "${!REPL[@]}"; do
    new="${REPL[$old]}"
    grep -qF "$old" "$file" && sed -i '' "s|$old|$new|g" "$file"
  done
done

echo "✅ xtensor include replacements complete."
