## lidR v2.0.2

- Fix: [#222](https://github.com/Jean-Romain/lidR/issues/222) `grid_*()` function return always a `RasterLayer` instead of a `RasterStack` if there is a single layer. VRT were returned as `RasterStack` no matter the nuber of layers.
- Fix: [#223](https://github.com/Jean-Romain/lidR/issues/223) `lasmergespatial()` wrongly copied shapefile attributes to each point when the paramter `attribute` was the name of an attribute of the shapefile.
- Fix: [#225](https://github.com/Jean-Romain/lidR/issues/225) `laspulse()`, `lasflightline()`, `lasscanline()` were broken since v2.0.0.
- Fix: [#227](https://github.com/Jean-Romain/lidR/issues/227) When processing a LAScatalog the chunks are better computed. In former version it was possible to have chunks that lie on tile only because of the buffer. These chunks are not build anymore.
- Fix: [#227](https://github.com/Jean-Romain/lidR/issues/227) When processing a LAScatalog we can discover that some chunk belong on the tiles but when reading the point there are point only in the buffer of the chunk. In these case an empty point cloud is returned and the computation must be skipped.
- Fix: [#228](https://github.com/Jean-Romain/lidR/issues/228) `lasmergespatial()` and `lasclip()` loose precision when extracting polygons due to missing digits in the WKT string used to build the polygons at C++ level.

## lidR v2.0.1 (Release date: 2010-02-02)

- Change: the function `catalog` has been slightly modified in prevision of the release of the package `rlas 1.3.0` to preserve future compatibility. This is invisible for the users.
- New: `lasnormalize` gained a parameter `na.rm = TRUE`
- Fix: an error occurend when plotting a LAScatalog with the option `chunk_pattern = TRUE`: objet 'ctg' introuvable.
- Fix: examples in documentation of `tin()` and `knnidw()` were inverted.
- Fix: [#213](https://github.com/Jean-Romain/lidR/issues/213) bug when using option `keep_lowest` in `grid_terrain`.
- Fix: [#212](https://github.com/Jean-Romain/lidR/issues/212) bug when merging big rasters that exeed the memory allowed by the raster package
- Fix: bug when merging rasters when some of then only have one cell
- Fix: bug when printing a 0 point LAS object

# lidR v2.0.0 (Release date: 2019-01-02)

### Why versions `> 2.0` are incompatible with versions `1.x.y`?

The lidR package versions 1 were mainly built upon "personal R scripts" I wrote 3 years ago. These scripts were written for my own use at a time when the lidR package was much smaller (both in term of code and users). The lidR package became a relatively large framework built on top of an unstructured base so it became impossible to develop it further. Many features and functions were missing because the way lidR was built did not allow them to be written. The new release (lidR version 2) breaks the former code to build a more robust, more consistent and more scalable framework that is intended and expected to continue for years without the need to break anything more in the future.

Old binaries can still be found here for 6 months:

- [Windows R-3.6](https://drive.google.com/open?id=1VHM38ftV71lasTQCcm8BxEChP2qxpV9e)
- [Windows R-3.5](https://drive.google.com/open?id=1E-Iieu0DTKHsDq1TDwq_VUGeiXta2T_Y)
- [Windows R-3.4](https://drive.google.com/open?id=1nRUyH0SaBCflGDCItkZhhSU9NQR0DXqr)
- [Mac OSX R-3.5](https://drive.google.com/open?id=1fmMTi6haFQpJFjAVVqW-vIFTULT_YWak)
- [Mac OSX R-3.4](https://drive.google.com/open?id=1Pw04QHeoF4uE8QPRIASGX_sB17r-sdZm)

### Overview of the main visible changes

**lidR as a GIS tool**

lidR versions 1 was not a GIS tool. For example, rasterization functions such as `grid_metrics()` or `grid_canopy()` returned a `data.frame`. Tree tops extraction with `tree_detection()` also returned a `data.frame`. Tree segmentation with `lastrees()` accepted `RasterLayer` or `data.frame` as input in a very inconsistent way. Moreover, the CRS of the point cloud was useless and never propagated to the outputs because outputs were not spatial objects.

lidR version 2 consistently uses `Raster*` and `Spatial*` objects everywhere. Rasterization functions such as `grid_metrics()` or `grid_canopy()` return `Raster*` objects. Tree tops extraction returns `SpatialPointDataFrame` objects. Tree segmentation methods accept `SpatialPointDataFrame` objects only in a consistent way across functions. The CRS of the point cloud is always propagated to the outputs. `LAS` objects are `Spatial` objects. `LAScatalog` objects are `SpatialPolygonDataFrame` objects. In short, lidR version 2 is now a GIS tool that is fully compatible with the R ecosystem.

**No longer any update by reference**

Several lidR functions used to update objects by reference. In lidR versions 1 the user wrote: `lasnormalize(las)` instead of `las2 <- lasnormalize(las1)`. This used to make sense in R < 3.1 but now the gain is no longer as relevant because R makes shallow copies instead of deep copies. 

To simplfy, let's assume that we have a 1 GB `data.frame` that stores the point cloud. In R < 3.1 `las2` was a copy of `las1` i.e. `las1` + `las2` = 2GB . This is why we made functions that worked by reference that implied no copy at all. This was memory optimized but not common or traditional in R. The question of memory optimization is now less relevant since R >= 3.1. In the previous example `las2` is no longer a deep copy of `las1`, but a shallow copy. Thus lidR now consistently uses the traditional syntax `y <- f(x)`.

**Algorithm dispatch**

The frame of lidR versions 1 was designed at a time when there were fewer algorithms. The increasing number of algorithms led to inconsistent ways to dispatch algorithms. For example:

* `grid_canopy()` implemented one algorithm and a second function `grid_tincanopy()` was created to implement another algorithm. With two functions the switch was possible by using two different names (algorithms dispatched by names).
* `grid_tincanopy()` actually implemented two algorithms in one function. The switch was possible by changing the input parameters in the function (algorithm dispatched by input).
* `lastrees()` had several variants that provided access to several algorithms: `lastrees_li()`, `lastrees_dalpontes()`, `lastrees_watershed()`, and so on. With several functions the switch was possible by using several different names (algorithms dispatched by names).
* `tree_detection` did not have several variants, thus it was impossible to introduce a new algorithm (no dispatch at all).

lidR version 2 comes with a flexible and scalable dispatch method that unifies all the former functions. For example, `grid_canopy()` is the only function to make a CHM. There is no longer the need for a second function `grid_tincanopy()`. `grid_canopy()` unifies the two functions by accepting as input an algorithm for a digital surface model:

```r
chm = grid_canopy(las, res = 1, algo = pitfree())
chm = grid_canopy(las, res = 1, algo = p2r(0.2))
```

The same idea drives several other functions including `lastrees`, `lassnags`, `tree_detection`, `grid_terrain`, `lasnormalize`, and so on. Examples:

```r
ttops = tree_detection(las, algo = lmf(5))
ttops = tree_detection(las, algo = lidRplugins::multichm(1,2))
lastrees(las, algo = li2012(1.5, 2))
lastrees(las, algo = watershed(chm))
lasnormalize(las, algo = tin())
lasnormalize(las, algo = knnidw(k = 10))
```

This allows `lidR` to be extended with new algorithms without any restriction either in lidR or even from third-party tools. Also, how lidR functions are used is now more consistent across the package.

**LAScatalog processing engine**

lidR versions 1 was designed to run algorithms on medium-sized point clouds loaded in memory but not to run algorithms over a set of files covering wide areas. In addition, lidR 1 had a poorly and inconsistently designed engine to process catalogs of las files. For example:

* It was possible to extract a polygon of points from a `LAScatalog` but not multipart-polygons or polygons with holes. This was only possible with `LAS` objects i.e loaded in memory (inconsistent behaviors within a function).
* It was possible to run `grid_metrics()` on a `LAScatalog` i.e. over a wide area not loaded in memory, but not `lasnormalize`, `lasground` or `tree_detection` (inconsistent behavior across the functions).

lidR version 2 comes with a powerful and scalable catalog processing engine. Almost all the lidR functions can be used seamlessly with either `LAS` or `LAScatalog` objects. The following chunks of code are now possible:

```r
ctg = catalog("folfer/to/las/file")
opt_output_file(ctg) <- "folder/to/normalized/las/files/{ORIGINALFILENAME}_normalized"
new_ctg = lasnormalize(ctg, algo = tin())
```

### Complete description of visible changes

**LAS class**

* Change: the `LAS` class **is** now a `Spatial` object or, more technically, it inherits a `Spatial` object.
* Change: being a `Spatial` object, a `LAS` object no longer has a `@crs` slot. It has now a slot `@proj4string` that is accessible with the functions `raster::projection` or `sp::proj4string`
* New: being a `Spatial` object, a `LAS` object inherits multiple functions from `raster` and `sp`, such `$` and `[[` accessors or `raster::extent`, `sp::bbox`, `raster::projection`, and so on. However, the replacement method `$<-`, `[[<-` have restricted capabilities to ensure a `LAS` object cannot be modified in a way that implies loosening the properties of the LAS specifications.
* New: empty `LAS` objects with 0 points are now allowed. This has repercussions for several functions including `lasfilter`, `lasclip`, and `readLAS` that do not return `NULL` for empty data but a `LAS` object with 0 points. This new behavior has been introduced to fix the old inconsistent behavior of functions that return either `LAS` or `NULL` objects. `LAS` objects are always returned.

**LAScatalog class**

* Change: the `LAScatalog` class **is** now a `SpatialPolygonsDataFrame` or, more technically, it inherits a `SpatialPolygonsDataFrame`. 
* Change: being a `SpatialPolygonsDataFrame` object, a `LAScatalog` no longer has a `@crs` slot. It has now a slot `@proj4string` that is accessible with the functions `raster::projection` or `sp::proj4string`.
* Change: being a `SpatialPolygonsDataFrame` a `LAScatalog` can be plotted with `sp::spplot()`.
* Change: there are no longer any slots `@cores`, `@by_file`, `@buffer`, and so on. They are replaced by more generic and scalable slots `@processing_options`, `@output_options`, `@clustering_options` and `@input_options` that are list of options classified by their main roles.
* Change: documentation has been entirely rewritten to explain the whole potential of the class.
* Change: functions `by_file`, `progress`, `tiling_size`, `buffer` were replaced by `opt_chunk_size`, `opt_chunk_buffer`, `opt_progress`, and so on. These allow for a consistent set of functions that do not overlap with functions from `raster` or `sp`.
* Change: standard column names were renamed to make syntactically-valid names and for compatibility with `sp` functions.

**readLAS**

* Change: `readLAS` no longer supports option `PFC`. Users must use the functions `laspulse`, `lasflightlines` manually.

**lasclip**

* New: `lasclip` now works both with a `LAS` object and a `LAScatalog` object in a seamless and consistent way. There are no longer any differences between the capabilities of the `LAS` version or the `LAScatalog` one.
* New: `lasclip` support many geometries including multipart polygons and polygons with holes, both with a `LAS` object and a `LAScatalog` object.
* Change: The option `inside` has been removed for consistency because it cannot be safely supported both on `LAS` and `LAScatalog`.
* Change: The option `ofile` has been removed for consistency and this option in now managed by the `LAScatalog` processing engine. For example, one can extract ground inventories and write them in `laz` files automatically named after their center coordinates like this:

```r
ctg = catalog(folder)
output_files(ctg) <- "path/to/a/file_{XCENTER}_{YCENTER}"
laz_compression(ctg) <- TRUE
new_ctg = lasclipCircle(ctg, xc,yc, r)
```

* Change: documentation has been reviewed and extended
* Change: `lasclip` does not return `NULL` anymore for empty queries but an empty `LAS` object.
* Fix: `lasclipRectangle` returns the same output both with a `LAS` and a `LAScatalog`. With a `LAS` the rectangle is now closed on the bottom and the left and open on the right and the top.

**catalog_queries**

* Change: `catalog_queries` has been removed because it is superseded by `lasclip`.

**lasnormalize**

* Change: `lasnormalize()` no longer updates the original object by reference.
* Change: remove the old option `copy = TRUE` that is now meaningless.
* Change: `lasnormalize()` now relies on lidR algorithms dispatch (see also the main new features above).
* New: `lasnormalize()` can be applied on a `LAScatalog` to write a new normalized catalog using the catalog processing engine (see also the main new features above).

**lasclassify**

* Change: `lasclassify()` is now named `lasmergespatial()` to free the name `lasclassify` that should be reserved for other usage.
* Change: `lasmergespatial()` no longer updates the original object by reference.
* Fix: the classification, when made with a `RasterLayer`, preserves the data type of the `RasterLayer`. This also fixes the fact that `lastrees()` used to classify the tree with `double` instead of `int`.

**tree_detection**

* Change: `tree_detection()` now relies on the new dispatch method (see also the main new features above).
* New: algorithm `lmf` has user-defined variable-sized search windows and two possible search window shapes (square or disc).
* New: introduction of the `manual` algorithm for manual correction of tree detection.
* New: `tree_detection` algorithms are seamlessly useable with a `LAScatalog` object by using the catalog processing engine (see also the main new features above). Thus, the following just works:

```r
ctg  <- catalog(folder)
ttop <- tree_detection(ctg, lmf(5))
```

* Change: the `lmf` algorithm, when used with a `RasterLayer` as input, expects parameters given in the units of the map and no longer in pixels.
* Change: `tree_detection()` function consistently returns a `SpatialPointsDataFrame` whatever the algorithm.
* Change: `tree_detection()` function based on a CHM no longer support a `lasmetric` object as input. Anyway, this class no longer exists.

**tree_metrics**

* Change: `tree_metrics()` returns a `SpatialPointsDataFrame`.
* Change: `tree_metrics()` is seamlessly useable with a `LAScatalog` using the catalog processing engine (see also the main new features above). Thus, this just works if the las file has extra bytes attributes that store the tree ids:

```r
ctg <- catalog(folder)
metrics <- tree_metrics(ctg, list(`Mean I` = mean(Intensity)))
```

**lastrees**

* Change: `lastrees()` now relies on the new algorithms dispatch method (see also the main new features above).
* New: introduction of the `mcwatershed` algorithm that implements a marker-controlled watershed.

**grid_metrics**

* Change: `grid_metrics()` as well as other `grid_*` functions consistently return a `RasterLayer` or a `RasterBrick` instead of a `data.table`.
* Change: option `splitlines` has been removed. `grid_metrics()` used to return a `data.table` because of the `splitlines` option and lidR was built on top of that feature from the very beginning. Now lidR consistently uses`sp` and `raster` and this option is no longer supported.

**grid_terrain**

* Change: `grid_terrain()` now relies on the new algorithms dispatch method (see also the main new features above).
* Change: `grid_terrain()` consistently returns a `RasterLayer` instead of a `data.table`, whatever the algorithm used.

**grid_canopy**

* Change: `grid_canopy()` now relies on the new algorithms dispatch method (see also the main new features above). It unifies the former functions `grid_canopy()` and `grid_tincanopy()`.
* Change: `grid_canopy()` consistently returns a `RasterLayer` instead of a `data.table`, whatever the algorithm used.
* Fix: the pitfree algorithm fails if a layer contains only 1 or 2 points.
* Fix: the p2r algorithm is five times faster with the subcircle tweak.

**grid_tincanopy**

* Change: `grid_tincanopy()` has been removed. Digital Surface Models are consistently driven by the function `grid_canopy()` and the lidR algorithm dispatch engine. The algorithms that replaced `grid_tincanopy()` are `dsmtin` and `pitfree`.

**grid_hexametrics**

* Change: as for `grid_metrics`, the parameter `splitlines` has been removed.
* Change: the function returns a `hexbin` object or a list of `hexbin` objects and no longer `data.table` objects.

**grid_catalog**

* Change: `grid_catalog()` has been removed. The new `LAScatalog` processing engine means that this function is no longer useful.

**class lasmetrics**

* `data.table` with a class `lasmetrics` no longer exists. It has been consistently replaced by `RasterLayer` and `RasterBrick` everywhere.
* `as.raster` no longer exists because it used to convert `lasmetrics` into `RasterLayer` and `RasterStack`.
* `as.spatial` no longer converts `lasmetrics` to `SpatialPixelsDataFrame` but still converts `LAS` to `SpatialPointsDataFrame`. 
* `plot.lasmetrics` has been removed obviously.

**lasroi**

* Change: `lasoi()` has been removed. It was not useful and 'buggy'. It might be reintroduced later in `lasclipManual`.

**lascolor**

* Change: `lascolor()` has been removed. It was one of the first functions of the package and is no longer useful because `plot()` has enhanced capabilities.

**lasfilterdecimate**

* Change: now relies on the new algorithms dispatch method (see also the main new features above).
* New: introduction of the algorithm `highest` available in `lasfilterdecimate()`. This supersedes the function `lasfiltersurfacepoints()`.

**lassnags**

* Change: `lassnags()` now relies on the new algorithms dispatch method (see also the main new features above).
* New: `lasnsnags()` can be applied on a `LAScatalog` to write a new catalog using the catalog processing engine (see also the main new features above).

**lidr_options**

* Change: `lidr_option()` has been removed. The options are now managed by regular R base options with function `options()`. Available lidR options are named with the prefix `lidR`.

**Example files**

* New: the three example files are now georeferenced with an EPSG code that is read and converted to a `proj4string`.
* New: the example file `MixedConifers.laz` contains the segmented trees in extra bytes 0.

**plot**

* New: `plot()` for `LAS` objects supports `RGB` as a color attribute.
* New: option `color` supports lazy evaluation. This syntax is correct: `plot(las, color = Classification)`.
* New: option `clear_artifact = TRUE` shifts the point cloud to (0,0) and reduces the display artifact due to the use of floating point in `rgl`.
* New: new functions `add_treetops3d`, `add_dtm3d` and `plot_dtm3d` add elements in the point cloud.
* Change: `trim` does not trim on a percentile of values but on the values themselves.

**Coordinate reference system**

* New: coordinate reference system is supported everywhere and can be written in las files. See function `epsg()`.
* New: function `lastranform` that returns transformed coordinates of a `LAS` object using the CRS argument.

**New functions**

* New: function `lasfilterduplicates`
* New: function `lascheck`
* New: function `lasvoxelize`

### Other changes that are not directly visible

* Change: the code that drives the `point_in_polygon` algorithm relies on `boost` and drastically simplifies the former code of `lasmergespatial()`
* Change: many memory optimizations

## lidR v1.6.1 (2018-08-21)

#### BUG FIXES

* [[#161](https://github.com/Jean-Romain/lidR/pull/161)] Fix tree ID matching.
* Fix undefined variable in cluster_apply on mac and linux if multicore processing is used.
* Fix rare case of unit test failure due to the random nature of the test dataset using seeds.
* [[#165](https://github.com/Jean-Romain/lidR/pull/165)] Unexported function in `catalog_apply` on Windows.

## lidR v1.6.0 (2018-07-20)

#### NEW FEATURE

* New function `tree_hulls` that computes a convex or concave hull for each segmented tree.
* New option `stop_early` that enables processing of an entire catalog or stops if an error occurs.
* New function `catalog_retile` supersedes the function `catalog_reshape` and performs the same task while adding much more functionality.


#### ENHANCEMENTS

* When processing a `LAScatalog`, error handling has been seriously improved. A process can now run until the end even with errors. In this case clusters with errors are skipped.
* When processing  a `LAScatalog`, the graphical progress now uses 3 colors. green: ok, red: error, gray: null.
* `as.spatial()` for `LAS` object preserves the CRS.
* All the functions now have strong assertions to check user inputs.
* `plot.LAScatalog` always displays the catalog with `mapview` by default even if the CRS is empty.
* In `lastrees_dalponte` the matching between the seeds and the canopy is more tolerant. Rasters can have different resolution and/or extent.
* `lasground` uses (as an option) only the last and single returns to perform the segmentation.

#### OTHER CHANGES

* `catalog()` displays a message when finding overlaps between files.
* The LAScatalog class is more thoroughly documented.
* Clusters now align on (0,0) by default when processing a `LAScatalog` by cluster.

#### BUG FIXES

* `lasscanline()` did not compute the scanline because the conditional statement that checked if the field was properly populated was incorrect.
* [[#146](https://github.com/Jean-Romain/lidR/issues/146)] Fix matching between tree tops, raster and canopy raster.
* `tree_detection` when used with a point cloud was not properly coded and tended to miss some trees.
* In `lasclip*` if `ofile` was non empty, the function wrote properly the file but returned a non-expected error.
* [[#155](https://github.com/Jean-Romain/lidR/issues/155)] user supplied function was being analyzed by `future` and some function were missing. User supplied function is now manually analyzed.
* [[#156](https://github.com/Jean-Romain/lidR/pull/156)] Fix error when `lasclip` was used with a `SpatialPolygonDataFrame`.

## lidR v1.5.1 (2018-06-14)

#### BUG FIXES

* The area of a `LAScatalog` was wrongly computed for non square tiles because of a bad copy/paste in the code.
* [[#135](https://github.com/Jean-Romain/lidR/issues/135)] Fix `NULL` class objects returned by `grid_*` functions when processing a `LAScatalog` if the first cluster is empty.
* [[#143](https://github.com/Jean-Romain/lidR/issues/143)] `rumple_index` returns `NA` if not computable.

## lidR v1.5.0 (2018-05-13)

#### SIGNIFICANT CHANGES

* `catalog_options()` is formally deprecated. Use `LAScatalog` properties instead (see `?catalog`).
* The package `magrittr` is no longer loaded with `lidR`. Thus, piping operators are no longer usable by default. To use piping operators use `library(magrittr)`.

#### NEW FEATURES

* New `lassmooth` function. A point cloud-based smoothing function.
* New `lasfiltersurfacepoints` function to filter surface points.
* New `grid_catalog` function is a simplified and more powerful function like `catalog_apply` but specifically dedicated to `grid_*` outputs.
* New functions `lasadddata`, `lasaddextrabyte` and `lasaddextrabyte_manual` to add new data in a `LAS` object.
* `lasclip` can clip a `SpatialPolygonsDataFrame`
* `lasclipRectangle` and `lasclipCircle` can clip multiple selections (non-documented feature).
* The `treeID` computed with `lastrees_*` functions can now be written in a `las/laz` file by default.

#### OTHER CHANGES

* `LAScatalog` objects are processed with a single core by default.
* `lasdecimate` is formally deprecated. Use `lasfilterdecimate`
* `grid_density` now returns both the point and the pulse density, where possible.
* The option `P` is no longer set by default in `readLAS`.
* The documentation of `lastrees` has been split into several pages.
* When a catalog is processed using several cores, if an error is raised the process triggers an early signal to stop the loop. In previous releases the entire process was run and the error was raised at the end when the futures were evaluated.

#### BUG FIXES

* `grid_metrics(lidar, stdmetrics_i(Intensity))` returned and empty `data.table`
* [[#128](https://github.com/Jean-Romain/lidR/issues/128)] Fix raster data extraction using the slower and memory-greedy, but safer `raster::extract` function.
* [[#126](https://github.com/Jean-Romain/lidR/issues/126)] propagate the CRS in filter functions.
* [[#116](https://github.com/Jean-Romain/lidR/issues/116)] Fix clash between function `area` from `lidR` and from `raster`.
* [[#110](https://github.com/Jean-Romain/lidR/issues/110)] Fix out-of-bounds rasterization.

## lidR v1.4.2 (2018-04-19)

#### BUG FIXES

* [[#103](https://github.com/Jean-Romain/lidR/issues/103)] fix user-defined function not exported in clusters on Windows
* [[#104](https://github.com/Jean-Romain/lidR/pull/104)] fix potential bin exclusion in `entropy` function
* [[#106](https://github.com/Jean-Romain/lidR/issues/106)] fix wrong count of points below 0
* Fix wrong type attribution in `lasclassify` when using the shapefile table of attributes as data.
* Fix column addition when `field = NULL` in `lasclassify`.
* Fix `NA` return in entropy when negative value are found.

#### NEW FEATURES

* Li et al algorithm has a new parameter Zu (see reference) that is no longer hard coded.

## lidR v1.4.1 (2018-02-01)

####  OTHER CHANGES

* Removed examples and unit tests that imply the watershed segmentation to make CRAN check happy with the new rules relative to bioconductor packages.

#### NEW FEATURES

* Parameter `start` has been enabled in `grid_metrics` with `catalogs`.

## lidR v1.4.0 (2018-01-24)

#### NEW FEATURES

* `lasclip` and `lasclip*` can extract from a catalog.
* `lasclip` supports `sp::Polygon` objects.
* `lastrees` gains a new algorithm from Silva et al. (2016).
* `lastrees` with the Li et al. (2012) algorithm gains a new parameter to prevent over-segmentation.
* new function `lassnags` for classifying points as snag points or for segmenting snags.
* new function `tree_detection` to detect individual trees. This feature has been extracted from `lastrees`'s algorithms and it is now up to the users to use `lidR`'s algos or other input sources.
* `plot` supports natively the `PointCloudViewer` package available on github.

#### BUG FIXES

* Fix missing pixel in DTM that made normalization impossible.
* [[#80](https://github.com/Jean-Romain/lidR/issues/80)] fix segfault.
* [[#84](https://github.com/Jean-Romain/lidR/issues/84)] fix bug in `lasscanline`.

#### ENHANCEMENTS
* `lastrees` with the Li et al. (2012) algorithm is now 5-6 times faster and much more memory efficient.
* `lastrees` with the Li et al. (2012) algorithm no longer sorts the original point cloud.
* `lastrees` with the Dalponte et al (2016) algorithm is now computed in linear time and is therefore hundreds to millions times faster.
* `catalog_reshape()` streams the data and uses virtually zero memory to run.
* `grid_canopy()` has been rewritten entirely in C++ and is now 10 to 20 times faster both with the option `subcircle` or without it.
* `grid_canopy()` with the option `subcircle` uses only 16 bytes of extra memory to run, while this feature previously required the equivalent of several copies of the point cloud (several hundreds of MB).
* `as.raster()` is now three times faster.
* `lasclassify` now uses a QuadTree and is therefore faster. This enables several algorithms to run faster, such as `lastrees` with Silva's algo.

#### OTHER CHANGES

* `lasground` with the PMF algorithm now accepts user-defined sequences.
* `lasground` with the PMF algorithm has simplified parameter names to make them easier to type and understand, and to prepare the package for new algorithms.
* `lasground` documentation is more explicit about the actual algorithm used.
* `lasground` now computes the windows size more closely in line with the original Zhang paper.
* `lastrees` when used with raster-based methods now accepts a missing las object. In that case `extra` is turned to `true`.
*  new parameter `p` (for power) added to functions that enable spatial interpolation with IDW.

## lidR v1.3.1 (Release date: 2017-09-20)

#### BUG FIXES

* Fix a bug of computer precision leading to non interpolated pixels at the boundaries of the QuadTree.

## lidR v1.3.0 (Release date: 2017-09-16)

This version is dedicated to extending functions and processes to entire catalogs in a continuous way.
Major changes are:

* How `catalog_apply` works. More powerful but no longer compatible with previous releases
* Former existing functions that now natively support a `Catalog` 
* Management of buffered areas

#### NEW FEATURES

* `catalog_apply` has been entirely re-designed. It is more flexible, more user-friendly and enables loading of buffered data.
* `catalog_queries` has now an argument `...` to pass any argument of `readLAS`.
* `catalog_queries` has now an argument `buffer` to load extra buffered points around the region of interest.
* `grid_metrics` accepts a catalog as input. It allows users to grid an entire catalog in a continuous way.
* `grid_density` also inherits this new feature
* `grid_terrain` also inherits this new feature
* `grid_canopy` also inherits this new feature
* `grid_tincanopy` also inherits this new feature
* `grid_metrics` has now has an argument `filter` for streaming filters when used with a catalog
* New function `catalog_reshape`

#### OTHER CHANGES

* `lasnormalize` updates the point cloud by reference and avoids making deep copies. An option `copy = TRUE` is available for compatibility with former versions.
* `readLAS` arguments changed. The new syntax is simpler. The previous syntax is still supported.
* `catalog_index` is no longer an exported function. It is now an internal function.
* `plot.Catalog` accepts the usual `plot` arguments
* `catalog_queries` and `catalog_apply` do not expect a parameter `mc.cores`. This is now driven by global options in `catalog_options()`.
* `grid_metrics` and `lasmetrics` do not expect a parameter `debug`. This is now driven by global options in `lidr_options`.
* `catalog` can build a catalog from a set of paths to files instead of a path to a folder.
* removed `$` access to LAS attribute (incredibly slow)
* `catalog_select` is more pleasant an more interactive to use.
* S3 `Catalog` class is now a S4 `LAScatalog` class
* `LAS` and `LAScatalog` class gain a slot `crs` automatically filled with a proj4 string
* `plot.LAScatalog` display a google map background if the catalog has a CRS.
* `plot.LAScatalog` gains an argument `y` to display a either a terrain, road, satellite map.
* `lasarea` is deprecated. Use the more generic function `area`

#### BUG FIXES

* Computer precision errors lead to holes in raster computed from a Delaunay triangulation.
* Message in `writeLAS` for skipped fields when no field is skipped is now correct.

#### ENHANCEMENTS

* `grid_terrain` with delaunay allocates less memory, makes fewer deep copies and is 2 to 3 times faster
* `grid_terrain` with knnidw allocates less memory, makes fewer deep copies and is 2 to 3 times faster
* `lasnormalize` and `lasclassify` no longer rely on `raster::extract` but on internal `fast_extract`, which is memory efficient and more than 15 times faster.
* `catalog` enables a `LAScatalog` to be built 8 times faster than previously.
* removed dependencies to `RANN` package using internal k-nearest neighbor search (2 to 3 times faster)

## lidR v1.2.1 (Release date: 2017-06-12)

#### NEW FEATURES

* new function `tree_metrics`.
* new function `stdtreemetrics`.
* `grid_tincanopy()` gains a parameter `subcircle` like `grid_canopy()`
* new function `rumple_index` for measuring roughness of a digital model (terrain or canopy)
* global options to parameterize the package - available with `lidr_options()`

#### BUG FIXES

* Installation fails if package sp is missing.
* Memory leak in QuadTree algorithm. Memory is now free after QuadTree deletion.
* Dalponte's algorithm had a bug due to the use of std::abs which works with integers. Replaced by std::fabs which works with doubles.
* In `grid_tincanopy` `x > 0` was replaced by `x >= 0` to avoid errors in the canopy height models
* Triangle boundaries are now taken into account in the rasterization of the Delaunay triangulation

#### OTHER CHANGES

* `lastrees` Li et al. algorithm for tree segmentation is now ten to a thousand of times faster than in v1.2.0
* `grid_terrain`, the interpolation is now done only within the convex hull of the point cloud
* `grid_tincanopy` makes the triangulation only for highest return per grid cell.
* `grid_tincanopy` and `grid_terrain` using Delaunay triangulation is now ten to a hundred times faster than in v1.2.0
* `as.raster` now relies on `sp` and is more flexible
* `as.raster` automatically returns a `RasterStack` if no layer is provided.
* `plot.lasmetrics` inherits `as.raster` changes and can display a `RasterStack`

## lidR v1.2.0 (Release date: 2017-03-26)

#### NEW FEATURES

* new function `lasground` for ground segmentation.
* new function `grid_tincanopy`. Canopy height model using Khosravipour et al. pit-free algorithm.
* new function `grid_hexametrics`. Area-based approach in hexagonal cells.
* `lasnormalize` allows for "non-discretized" normalization i.e interpolating each point instead of using a raster.
* internally `lascheck` performs more tests to check if the header is in accordance with the data.

#### BUG FIXES

* [[#48](https://github.com/Jean-Romain/lidR/pull/48)] `gap_fraction_profile()` bug with negative values (thanks to Florian de Boissieu)
* [[#49](https://github.com/Jean-Romain/lidR/pull/49)] typo error leading to the wrong metric in `stdmetric_i` 
* [[#50](https://github.com/Jean-Romain/lidR/pull/50)] typo error leading to the wrong metric in `stdmetric` 
* Fix bug in `stdmetric_z` when `max(Z) = 0`
* [[#54](https://github.com/Jean-Romain/lidR/pull/54)] better re-computation of the header of LAS objects.

#### OTHER CHANGES

* Slightly faster point classification from shapefiles.
* [[#51](https://github.com/Jean-Romain/lidR/pull/51)] in `grid_terrain`, forcing the lowest point to be retained is now an option `keep_lowest = FALSE`

## lidR v1.1.0 (Release date: 2017-02-05)

#### NEW FEATURES

* `lastree()` for individual tree segmentation
* `readLAS()` gains a parameter `filter` from `rlas (>= 1.1.0)`
* `catalog_queries()` relies on `rlas (>= 1.1.0)`. It saves a lot of memory, is 2 to 6 times faster and supports .lax files.

#### OTHER CHANGES

* `colorPalette` parameter in `plot.LAS()` now expects a list of colors instead of a function. Use `height.colors(50)` instead of `height.colors`
* The header of a LAS object is now an S4 class object called `LASheader`
* The spatial interpolation method `akima` is now called `delaunay` because it corresponds to what is actually computed.
* The spatial interpolation method `akima` lost its parameter `linear`.
* The spatial interpolation method `kriging` now performs a KNN kriging.
* `catalog_queries()` lost the parameter `...` all the fields are loaded by default.
* Removed `lasterrain()` which was not consistent with other functions and not useful.

#### BUG FIXES

* The header of LAS objects automatically updates `Number of point records` and `Number of nth return`.
* `lasnormalize()` updates the header and returns warnings for some behaviors
* [[#39](https://github.com/Jean-Romain/lidR/issues/39)] interpolation with duplicated ground points


## lidR v1.0.2 (Release date: 2016-12-31)

Third submission

* Change: explain LiDAR in the Description - requested by Kurt Hornik.

## lidR v1.0.1 (Release date: 2016-12-30)

Second submission - rejected

* Change: split the package in two parts. 'lidR' relies on 'rlas' to read binary files.

# lidR v1.0.0 (Release date: 201-12-16)

First submission - rejected
