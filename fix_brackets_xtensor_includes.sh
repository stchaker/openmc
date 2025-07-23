#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR=${1:-.}
echo "Applying xtensor angle-bracket include replacements in: ${ROOT_DIR}"

find "$ROOT_DIR" -type f \
  \( -name "*.cpp" -o -name "*.hpp" -o -name "*.h" -o -name "*.cc" -o -name "*.cxx" -o -name "*.tpp" \) \
  -print0 | while IFS= read -r -d '' file; do

  # containers
  sed -i '' 's|<xtensor/xarray.hpp>|<xtensor/containers/xarray.hpp>|g' "$file"
  sed -i '' 's|<xtensor/xadapt.hpp>|<xtensor/containers/xadapt.hpp>|g' "$file"
  sed -i '' 's|<xtensor/xbuffer_adaptor.hpp>|<xtensor/containers/xbuffer_adaptor.hpp>|g' "$file"
  sed -i '' 's|<xtensor/xcontainer.hpp>|<xtensor/containers/xcontainer.hpp>|g' "$file"
  sed -i '' 's|<xtensor/xfixed.hpp>|<xtensor/containers/xfixed.hpp>|g' "$file"
  sed -i '' 's|<xtensor/xscalar.hpp>|<xtensor/containers/xscalar.hpp>|g' "$file"
  sed -i '' 's|<xtensor/xstorage.hpp>|<xtensor/containers/xstorage.hpp>|g' "$file"

  # chunk
  sed -i '' 's|<xtensor/xchunked_array.hpp>|<xtensor/chunk/xchunked_array.hpp>|g' "$file"
  sed -i '' 's|<xtensor/xchunked_assign.hpp>|<xtensor/chunk/xchunked_assign.hpp>|g' "$file"
  sed -i '' 's|<xtensor/xchunked_view.hpp>|<xtensor/chunk/xchunked_view.hpp>|g' "$file"

  # core
  sed -i '' 's|<xtensor/xaccessible.hpp>|<xtensor/core/xaccessible.hpp>|g' "$file"
  sed -i '' 's|<xtensor/xassign.hpp>|<xtensor/core/xassign.hpp>|g' "$file"
  sed -i '' 's|<xtensor/xeval.hpp>|<xtensor/core/xeval.hpp>|g' "$file"
  sed -i '' 's|<xtensor/xexpression.hpp>|<xtensor/core/xexpression.hpp>|g' "$file"
  sed -i '' 's|<xtensor/xexpression_traits.hpp>|<xtensor/core/xexpression_traits.hpp>|g' "$file"
  sed -i '' 's|<xtensor/xfunction.hpp>|<xtensor/core/xfunction.hpp>|g' "$file"
  sed -i '' 's|<xtensor/xiterable.hpp>|<xtensor/core/xiterable.hpp>|g' "$file"
  sed -i '' 's|<xtensor/xiterator.hpp>|<xtensor/core/xiterator.hpp>|g' "$file"
  sed -i '' 's|<xtensor/xlayout.hpp>|<xtensor/core/xlayout.hpp>|g' "$file"
  sed -i '' 's|<xtensor/xmath.hpp>|<xtensor/core/xmath.hpp>|g' "$file"
  sed -i '' 's|<xtensor/xmultiindex_iterator.hpp>|<xtensor/core/xmultiindex_iterator.hpp>|g' "$file"
  sed -i '' 's|<xtensor/xnoalias.hpp>|<xtensor/core/xnoalias.hpp>|g' "$file"
  sed -i '' 's|<xtensor/xoperation.hpp>|<xtensor/core/xoperation.hpp>|g' "$file"
  sed -i '' 's|<xtensor/xsemantic.hpp>|<xtensor/core/xsemantic.hpp>|g' "$file"
  sed -i '' 's|<xtensor/xshape.hpp>|<xtensor/core/xshape.hpp>|g' "$file"
  sed -i '' 's|<xtensor/xstrides.hpp>|<xtensor/core/xstrides.hpp>|g' "$file"
  sed -i '' 's|<xtensor/xtensor_config.hpp>|<xtensor/core/xtensor_config.hpp>|g' "$file"
  sed -i '' 's|<xtensor/xtensor_forward.hpp>|<xtensor/core/xtensor_forward.hpp>|g' "$file"
  sed -i '' 's|<xtensor/xvectorize.hpp>|<xtensor/core/xvectorize.hpp>|g' "$file"

  # generators
  sed -i '' 's|<xtensor/xbuilder.hpp>|<xtensor/generators/xbuilder.hpp>|g' "$file"
  sed -i '' 's|<xtensor/xgenerator.hpp>|<xtensor/generators/xgenerator.hpp>|g' "$file"
  sed -i '' 's|<xtensor/xrandom.hpp>|<xtensor/generators/xrandom.hpp>|g' "$file"

  # io
  sed -i '' 's|<xtensor/xcsv.hpp>|<xtensor/io/xcsv.hpp>|g' "$file"
  sed -i '' 's|<xtensor/xinfo.hpp>|<xtensor/io/xinfo.hpp>|g' "$file"
  sed -i '' 's|<xtensor/xio.hpp>|<xtensor/io/xio.hpp>|g' "$file"
  sed -i '' 's|<xtensor/xjson.hpp>|<xtensor/io/xjson.hpp>|g' "$file"
  sed -i '' 's|<xtensor/xmime.hpp>|<xtensor/io/xmime.hpp>|g' "$file"
  sed -i '' 's|<xtensor/xnpy.hpp>|<xtensor/io/xnpy.hpp>|g' "$file"

  # misc
  sed -i '' 's|<xtensor/xcomplex.hpp>|<xtensor/misc/xcomplex.hpp>|g' "$file"
  sed -i '' 's|<xtensor/xexpression_holder.hpp>|<xtensor/misc/xexpression_holder.hpp>|g' "$file"
  sed -i '' 's|<xtensor/xfft.hpp>|<xtensor/misc/xfft.hpp>|g' "$file"
  sed -i '' 's|<xtensor/xhistogram.hpp>|<xtensor/misc/xhistogram.hpp>|g' "$file"
  sed -i '' 's|<xtensor/xmanipulation.hpp>|<xtensor/misc/xmanipulation.hpp>|g' "$file"
  sed -i '' 's|<xtensor/xpad.hpp>|<xtensor/misc/xpad.hpp>|g' "$file"
  sed -i '' 's|<xtensor/xset_operation.hpp>|<xtensor/misc/xset_operation.hpp>|g' "$file"
  sed -i '' 's|<xtensor/xsort.hpp>|<xtensor/misc/xsort.hpp>|g' "$file"
  sed -i '' 's|<xtensor/xtl_concepts.hpp>|<xtensor/misc/xtl_concepts.hpp>|g' "$file"

  # optional
  sed -i '' 's|<xtensor/xoptional.hpp>|<xtensor/optional/xoptional.hpp>|g' "$file"
  sed -i '' 's|<xtensor/xoptional_assembly.hpp>|<xtensor/optional/xoptional_assembly.hpp>|g' "$file"
  sed -i '' 's|<xtensor/xoptional_assembly_base.hpp>|<xtensor/optional/xoptional_assembly_base.hpp>|g' "$file"
  sed -i '' 's|<xtensor/xoptional_assembly_storage.hpp>|<xtensor/optional/xoptional_assembly_storage.hpp>|g' "$file"

  # reducers
  sed -i '' 's|<xtensor/xaccumulator.hpp>|<xtensor/reducers/xaccumulator.hpp>|g' "$file"
  sed -i '' 's|<xtensor/xblockwise_reducer.hpp>|<xtensor/reducers/xblockwise_reducer.hpp>|g' "$file"
  sed -i '' 's|<xtensor/xblockwise_reducer_functors.hpp>|<xtensor/reducers/xblockwise_reducer_functors.hpp>|g' "$file"
  sed -i '' 's|<xtensor/xnorm.hpp>|<xtensor/reducers/xnorm.hpp>|g' "$file"
  sed -i '' 's|<xtensor/xreducer.hpp>|<xtensor/reducers/xreducer.hpp>|g' "$file"

  # utils
  sed -i '' 's|<xtensor/xexception.hpp>|<xtensor/utils/xexception.hpp>|g' "$file"
  sed -i '' 's|<xtensor/xtensor_simd.hpp>|<xtensor/utils/xtensor_simd.hpp>|g' "$file"
  sed -i '' 's|<xtensor/xutils.hpp>|<xtensor/utils/xutils.hpp>|g' "$file"

  # views
  sed -i '' 's|<xtensor/xaxis_iterator.hpp>|<xtensor/views/xaxis_iterator.hpp>|g' "$file"
  sed -i '' 's|<xtensor/xaxis_slice_iterator.hpp>|<xtensor/views/xaxis_slice_iterator.hpp>|g' "$file"
  sed -i '' 's|<xtensor/xbroadcast.hpp>|<xtensor/views/xbroadcast.hpp>|g' "$file"
  sed -i '' 's|<xtensor/xdynamic_view.hpp>|<xtensor/views/xdynamic_view.hpp>|g' "$file"
  sed -i '' 's|<xtensor/xfunctor_view.hpp>|<xtensor/views/xfunctor_view.hpp>|g' "$file"
  sed -i '' 's|<xtensor/xindex_view.hpp>|<xtensor/views/xindex_view.hpp>|g' "$file"
  sed -i '' 's|<xtensor/xmasked_view.hpp>|<xtensor/views/xmasked_view.hpp>|g' "$file"
  sed -i '' 's|<xtensor/xoffset_view.hpp>|<xtensor/views/xoffset_view.hpp>|g' "$file"
  sed -i '' 's|<xtensor/xrepeat.hpp>|<xtensor/views/xrepeat.hpp>|g' "$file"
  sed -i '' 's|<xtensor/xslice.hpp>|<xtensor/views/xslice.hpp>|g' "$file"
  sed -i '' 's|<xtensor/xstrided_view.hpp>|<xtensor/views/xstrided_view.hpp>|g' "$file"
  sed -i '' 's|<xtensor/xstrided_view_base.hpp>|<xtensor/views/xstrided_view_base.hpp>|g' "$file"
  sed -i '' 's|<xtensor/xview.hpp>|<xtensor/views/xview.hpp>|g' "$file"
  sed -i '' 's|<xtensor/xview_utils.hpp>|<xtensor/views/xview_utils.hpp>|g' "$file"

done

echo "✅ xtensor angle-bracket includes updated successfully."
