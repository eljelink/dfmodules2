/**
 * @file HDF5Read_test.cxx Application that tests and demonstrates
 * the read functionality of the HDF5DataStore class.
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "HDF5DataStore.hpp"

#include "ers/ers.h"

#define BOOST_TEST_MODULE HDF5Read_test // NOLINT

#include <boost/test/unit_test.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <regex>
#include <string>
#include <vector>

using namespace dunedaq::dfmodules;

std::vector<std::string>
deleteFilesMatchingPattern(const std::string& path, const std::string& pattern)
{
  std::regex regexSearchPattern(pattern);
  std::vector<std::string> fileList;
  for (const auto& entry : std::filesystem::directory_iterator(path)) {
    if (std::regex_match(entry.path().filename().string(), regexSearchPattern)) {
      if (std::filesystem::remove(entry.path())) {
        fileList.push_back(entry.path());
      }
    }
  }
  return fileList;
}

BOOST_AUTO_TEST_SUITE(HDF5Read_test)

/*
BOOST_AUTO_TEST_CASE(ReadFragmentFiles)
{
  std::string filePath(std::filesystem::temp_directory_path());
  std::string filePrefix = "demo" + std::to_string(getpid());
  const int EVENT_COUNT = 2;
  const int GEOLOC_COUNT = 2;
  const int DUMMYDATA_SIZE = 128;

  // delete any pre-existing files so that we start with a clean slate
  std::string deletePattern = filePrefix + ".*.hdf5";
  deleteFilesMatchingPattern(filePath, deletePattern);

  // create the DataStore instance for writing
  nlohmann::json conf ;
  conf["name"] = "tempWriter" ;
  conf["filename_prefix"] = filePrefix ;
  conf["directory_path"] = filePath ;
  conf["mode"] = "one-fragment-per-file" ;
  std::unique_ptr<HDF5DataStore> dsPtr(new HDF5DataStore( conf ));

  // write several events, each with several fragments
  int initializedChecksum = 0;
  char dummyData[DUMMYDATA_SIZE];
  for (int idx = 0; idx < DUMMYDATA_SIZE; ++idx) {
    int val = 0x7f & idx;
    dummyData[idx] = val;
    initializedChecksum += val;
  }
  std::vector<StorageKey> keyList;
  for (int eventID = 1; eventID <= EVENT_COUNT; ++eventID) {
    for (int geoLoc = 0; geoLoc < GEOLOC_COUNT; ++geoLoc) {
      StorageKey key(eventID, StorageKey::s_invalid_detector_id, geoLoc);
      KeyedDataBlock dataBlock(key);
      dataBlock.m_unowned_data_start = static_cast<void*>(&dummyData[0]);
      dataBlock.m_data_size = DUMMYDATA_SIZE;
      dsPtr->write(dataBlock);
      keyList.push_back(key);
    }
  }
  dsPtr.reset(); // explicit destruction

  // create a new DataStore instance to read back the data that was written
  nlohmann::json read_conf ;
  read_conf["name"] = "tempReader" ;
  read_conf["filename_prefix"] = filePrefix ;
  read_conf["directory_path"] = filePath ;
  read_conf["mode"] = "one-fragment-per-file" ;
  std::unique_ptr<HDF5DataStore> dsPtr2(new HDF5DataStore(read_conf));

  // loop over all of the keys to read in the data
  for (size_t kdx = 0; kdx < keyList.size(); ++kdx) {
    KeyedDataBlock dataBlock = dsPtr2->read(keyList[kdx]);
    BOOST_REQUIRE_EQUAL(dataBlock.get_data_size_bytes(), DUMMYDATA_SIZE);

    const char* data_ptr = static_cast<const char*>(dataBlock.get_data_start());
    int readbackChecksum = 0;
    for (int idx = 0; idx < DUMMYDATA_SIZE; ++idx) {
      readbackChecksum += static_cast<int>(data_ptr[idx]);
    }
    BOOST_REQUIRE_EQUAL(readbackChecksum, initializedChecksum);
  }
  dsPtr2.reset(); // explicit destruction

  // clean up the files that were created
  deleteFilesMatchingPattern(filePath, deletePattern);
}

BOOST_AUTO_TEST_CASE(ReadEventFiles)
{
  std::string filePath(std::filesystem::temp_directory_path());
  std::string filePrefix = "demo" + std::to_string(getpid());
  const int EVENT_COUNT = 2;
  const int GEOLOC_COUNT = 2;
  const int DUMMYDATA_SIZE = 128;

  // delete any pre-existing files so that we start with a clean slate
  std::string deletePattern = filePrefix + ".*.hdf5";
  deleteFilesMatchingPattern(filePath, deletePattern);

  // create the DataStore instance for writing
  nlohmann::json conf ;
  conf["name"] = "tempWriter" ;
  conf["filename_prefix"] = filePrefix ;
  conf["directory_path"] = filePath ;
  conf["mode"] = "one-event-per-file" ;
  std::unique_ptr<HDF5DataStore> dsPtr(new HDF5DataStore( conf ));

  // write several events, each with several fragments
  int initializedChecksum = 0;
  char dummyData[DUMMYDATA_SIZE];
  for (int idx = 0; idx < DUMMYDATA_SIZE; ++idx) {
    int val = 0x7f & idx;
    dummyData[idx] = val;
    initializedChecksum += val;
  }
  std::vector<StorageKey> keyList;
  for (int eventID = 1; eventID <= EVENT_COUNT; ++eventID) {
    for (int geoLoc = 0; geoLoc < GEOLOC_COUNT; ++geoLoc) {
      StorageKey key(eventID, StorageKey::s_invalid_detector_id, geoLoc);
      KeyedDataBlock dataBlock(key);
      dataBlock.m_unowned_data_start = static_cast<void*>(&dummyData[0]);
      dataBlock.m_data_size = DUMMYDATA_SIZE;
      dsPtr->write(dataBlock);
      keyList.push_back(key);
    }
  }
  dsPtr.reset(); // explicit destruction

  // create a new DataStore instance to read back the data that was written
  nlohmann::json read_conf ;
  read_conf["name"] = "tempReader" ;
  read_conf["filename_prefix"] = filePrefix ;
  read_conf["directory_path"] = filePath ;
  read_conf["mode"] = "one-event-per-file" ;
  std::unique_ptr<HDF5DataStore> dsPtr2(new HDF5DataStore( read_conf  ));

  // loop over all of the keys to read in the data
  for (size_t kdx = 0; kdx < keyList.size(); ++kdx) {
    KeyedDataBlock dataBlock = dsPtr2->read(keyList[kdx]);
    BOOST_REQUIRE_EQUAL(dataBlock.get_data_size_bytes(), DUMMYDATA_SIZE);

    const char* data_ptr = static_cast<const char*>(dataBlock.get_data_start());
    int readbackChecksum = 0;
    for (int idx = 0; idx < DUMMYDATA_SIZE; ++idx) {
      readbackChecksum += static_cast<int>(data_ptr[idx]);
    }
    BOOST_REQUIRE_EQUAL(readbackChecksum, initializedChecksum);
  }
  dsPtr2.reset(); // explicit destruction

  // clean up the files that were created
  deleteFilesMatchingPattern(filePath, deletePattern);
}
*/
BOOST_AUTO_TEST_CASE(ReadSingleFile)
{
  std::string filePath(std::filesystem::temp_directory_path());
  std::string filePrefix = "demo" + std::to_string(getpid());

  const int DUMMYDATA_SIZE = 7;
  const int RUN_NUMBER = 52;
  const int TRIGGER_COUNT = 5;
  const std::string DETECTOR = "FELIX";
  const int APA_COUNT = 3;
  const int LINK_COUNT = 1;

  // delete any pre-existing files so that we start with a clean slate
  std::string deletePattern = filePrefix + ".*.hdf5";
  deleteFilesMatchingPattern(filePath, deletePattern);

  // create the DataStore instance for writing
  nlohmann::json conf;
  conf["name"] = "tempWriter";
  conf["directory_path"] = filePath;
  conf["mode"] = "all-per-file";
  nlohmann::json subconf;
  subconf["overall_prefix"] = filePrefix;
  conf["filename_parameters"] = subconf;
  std::unique_ptr<HDF5DataStore> dsPtr(new HDF5DataStore(conf));

  int initializedChecksum = 0;
  // write several events, each with several fragments
  char dummyData[DUMMYDATA_SIZE];
  for (int idx = 0; idx < DUMMYDATA_SIZE; ++idx) {
    int val = 0x7f & idx;
    dummyData[idx] = val;
    initializedChecksum += val;
  }
  std::vector<StorageKey> keyList;
  for (int triggerNumber = 1; triggerNumber <= TRIGGER_COUNT; ++triggerNumber) {
    for (int apaNumber = 1; apaNumber <= APA_COUNT; ++apaNumber) {
      for (int linkNumber = 1; linkNumber <= LINK_COUNT; ++linkNumber) {
        StorageKey key(RUN_NUMBER, triggerNumber, DETECTOR, apaNumber, linkNumber);
        KeyedDataBlock dataBlock(key);
        dataBlock.m_unowned_data_start = static_cast<void*>(&dummyData[0]);
        dataBlock.m_data_size = DUMMYDATA_SIZE;
        dsPtr->write(dataBlock);
        keyList.push_back(key);
      }          // link number
    }            // apa number
  }              // trigger number
  dsPtr.reset(); // explicit destruction

  // create a new DataStore instance to read back the data that was written
  nlohmann::json read_conf;
  read_conf["name"] = "tempReader";
  read_conf["directory_path"] = filePath;
  read_conf["mode"] = "all-per-file";
  nlohmann::json read_subconf;
  read_subconf["overall_prefix"] = filePrefix;
  read_conf["filename_parameters"] = read_subconf;
  std::unique_ptr<HDF5DataStore> dsPtr2(new HDF5DataStore(read_conf));

  // loop over all of the keys to read in the data
  for (size_t kdx = 0; kdx < keyList.size(); ++kdx) {
    KeyedDataBlock dataBlock = dsPtr2->read(keyList[kdx]);
    BOOST_REQUIRE_EQUAL(dataBlock.get_data_size_bytes(), DUMMYDATA_SIZE);
    const char* data_ptr = static_cast<const char*>(dataBlock.get_data_start());
    int readbackChecksum = 0;
    for (int idx = 0; idx < DUMMYDATA_SIZE; ++idx) {
      readbackChecksum += static_cast<int>(data_ptr[idx]);
    }
    BOOST_REQUIRE_EQUAL(readbackChecksum, initializedChecksum);
  }
  dsPtr2.reset(); // explicit destruction

  // clean up the files that were created
  deleteFilesMatchingPattern(filePath, deletePattern);
}

BOOST_AUTO_TEST_SUITE_END()
