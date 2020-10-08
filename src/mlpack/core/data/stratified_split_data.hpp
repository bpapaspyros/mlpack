/**
 * @file core/data/stratified_split_data.hpp
 * @author Anush Kini
 *
 * Defines StratifiedSplit(), a utility function to split a dataset into
 * stratified training and test sets.
 *
 * mlpack is free software; you may redistribute it and/or modify it under the
 * terms of the 3-clause BSD license.  You should have received a copy of the
 * 3-clause BSD license along with mlpack.  If not, see
 * http://www.opensource.org/licenses/BSD-3-Clause for more information.
 */
 #ifndef MLPACK_CORE_DATA_STRATIFIED_SPLIT_DATA_HPP
 #define MLPACK_CORE_DATA_STRATIFIED_SPLIT_DATA_HPP

 #include <mlpack/prereqs.hpp>

 namespace mlpack {
 namespace data {
/**
* Given an input dataset and labels, split into a training set and test set.
* Example usage below.  This overload places the split dataset into the four
* output parameters given (trainData, testData, trainLabel, and testLabel).
*
* @code
* arma::mat input = loadData();
* arma::Row<size_t> label = loadLabel();
* arma::mat trainData;
* arma::mat testData;
* arma::Row<size_t> trainLabel;
* arma::Row<size_t> testLabel;
* math::RandomSeed(100); // Set the seed if you like.
*
* // Split the dataset into a training and test set, with 30% of the data being
* // held out for the test set.
* Split(input, label, trainData,
*                testData, trainLabel, testLabel, 0.3);
* @endcode
*
* @param input Input dataset to split.
* @param inputLabel Input labels to split.
* @param trainData Matrix to store training data into.
* @param testData Matrix to store test data into.
* @param trainLabel Vector to store training labels into.
* @param testLabel Vector to store test labels into.
* @param testRatio Percentage of dataset to use for test set (between 0 and 1).
* @param shuffleData If true, the sample order is shuffled; otherwise, each
*       sample is visited in linear order. (Default true.)
*/
template<typename T, typename U>
void StratifiedSplit(const arma::Mat<T>& input,
										 const arma::Row<U>& inputLabel,
										 arma::Mat<T>& trainData,
                     arma::Mat<T>& testData,
                     arma::Row<U>& trainLabel,
                     arma::Row<U>& testLabel,
                     const double testRatio,
                     const bool shuffleData = true)
{
  if (shuffleData)
  {
    arma::uvec order = arma::shuffle(arma::linspace<arma::uvec>(
        0, input.n_cols - 1, input.n_cols));
    input = input.cols(order);
    inputLabel = inputLabel.cols(order);
  }
  arma::Row<U> uniqueLabel = arma::unique(inputLabel);
  //for (U )
}
