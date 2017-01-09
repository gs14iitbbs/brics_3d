/******************************************************************************
* BRICS_3D - 3D Perception and Modeling Library
* Copyright (c) 2013, KU Leuven
*
* Author: Sebastian Blumenthal
*
*
* This software is published under a dual-license: GNU Lesser General Public
* License LGPL 2.1 and Modified BSD license. The dual-license implies that
* users of this code may choose which terms they prefer.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU Lesser General Public License LGPL and the BSD license for
* more details.
*
******************************************************************************/

#ifndef BRICS_3D_TEMPORALCACHE_H_
#define BRICS_3D_TEMPORALCACHE_H_

#include "TimeStamp.h"
#include "brics_3d/core/Logger.h"
#include <vector>
#include <string>
#include <sstream>
#include <iomanip> // setprecision

using brics_3d::Logger;

namespace brics_3d {

namespace rsg {

/**
 * @brief A generic temporal cache.
 *
 * Example usage:
 *
 * @code
 *
 *  TemporalCache<int> cache(TimeStamp(20, Units::Second)); // cache size = 20[s]
 *
 *  // Note: in real applications the time stamps will be generated by a timer
 *  // but for demonstration purposes we will use manually defined time stamps.
 *
 *  cache.insertData(1, TimeStamp(0, Units::Second));    // entry has a value of 1
 *  cache.insertData(2, TimeStamp(10.0, Units::Second)); // entry has a value of 2
 *  cache.insertData(3, TimeStamp(15.0, Units::Second)); // entry has a value of 3
 *
 *  int result;
 *  result = cache.getData(TimeStamp(0, Units::Second)));   // result = 1
 *  result = cache.getData(TimeStamp(4, Units::Second)));  // result = 1;
 *  result = cache.getData(TimeStamp(5, Units::Second)));   // result = 1;
 *  result = cache.getData(TimeStamp(10, Units::Second)));  // result = 2
 *  result = cache.getData(TimeStamp(12, Units::Second)));  // result = 2
 *  result = cache.getData(TimeStamp(12.5, Units::Second)));// result = 2;
 *  result = cache.getData(TimeStamp(15, Units::Second)));  // result = 3
 *  result = cache.getData(TimeStamp(30, Units::Second)));  // result = 3; beyond cache limit -> still the latest entry will be returnd
 *
 *  // It is possible to change the access policy be explicitly passing it as a another parameter:
 *  result = cache.getData(TimeStamp(0, Units::Second), &TemporalCache<int>::getClosestData));   // result = 1
 *  result = cache.getData(TimeStamp(4, Units::Second), &TemporalCache<int>::getClosestData));   // result = 1;
 *  result = cache.getData(TimeStamp(5, Units::Second), &TemporalCache<int>::getClosestData));   // result = 2; equal distance to 0 and 10 -> the newer one will be selected
 *  result = cache.getData(TimeStamp(10, Units::Second), &TemporalCache<int>::getClosestData));  // result = 2
 *  result = cache.getData(TimeStamp(12, Units::Second), &TemporalCache<int>::getClosestData));  // result = 2
 *  result = cache.getData(TimeStamp(12.5, Units::Second), &TemporalCache<int>::getClosestData));// result = 3; equal distance to 10 and 15 -> the newer one will be selected
 *  result = cache.getData(TimeStamp(15, Units::Second), &TemporalCache<int>::getClosestData));  // result = 3
 *  result = cache.getData(TimeStamp(30, Units::Second), &TemporalCache<int>::getClosestData));  // result = 3; beyond cache limit -> still the latest entry will be returnd
 *
 *  // Default policy is "getPrecedingData"
 *  result = cache.getData(TimeStamp(0, Units::Second), &TemporalCache<int>::getPrecedingData));   // result = 1
 *  result = cache.getData(TimeStamp(0, Units::Second)));   // result = 1; equivalent to the above line.
 *
 *  // The caches can be also accessed via iterators:
 *  typedef std::vector<std::pair<int, TimeStamp> >::const_iterator IntCacheIterator; // Note that cache has decending order: The older, the closer to the end.
 *  for (IntCacheIterator iterator = cache.begin(); iterator!=cache.end(); ++iterator) {
 *  	std::cout << "iterator (value, stamp): (" << iterator->first << "," << iterator->second.getSeconds() << ")" << std::endl;
 *  	//iterator->first = 42; // does not work because access is read-only
 *  }
 *
 *  typedef std::vector<std::pair<int, TimeStamp> >::const_reverse_iterator IntCacheReverseIterator; // Go through values in ascending order.
 *  for (IntCacheReverseIterator iterator = cache.rbegin(); iterator!=cache.rend(); ++iterator) {
 *  	std::cout << "reverse iterator (value, stamp): (" << iterator->first << "," << iterator->second.getSeconds() << ")" << std::endl;
 *  }
 *
 * @endcode
 *
 * The resulting output would be:
 *
 * @verbatim

  iterator (value, stamp): (3,15)
  iterator (value, stamp): (2,10)
  iterator (value, stamp): (1,0)
  reverse iterator (value, stamp): (1,0)
  reverse iterator (value, stamp): (2,10)
  reverse iterator (value, stamp): (3,15)

   @endverbatim
 *
 */
template<typename T>
class TemporalCache {
public:

    /// Cache data type; for convenience
    typedef std::vector<std::pair<T, TimeStamp> > Cache;

    /// Expected signature for further access policies.
    typedef typename Cache::iterator (*CacheAccessFunctor)(TimeStamp, Cache&);

	/**
	 * @brief Constructor for a the temporal cache with optional cache size parameter.
	 * @param maxHistoryDuration Size of the cache. Default value is 10[s].
	 * In case of an invalid input, that is a negative value, it will be set to a duration of 0[s].
	 */
	TemporalCache(TimeStamp maxHistoryDuration = TimeStamp(10.0, Units::Second)) {
		this->setMaxHistoryDuration(maxHistoryDuration);
		history.clear();
	}

	/**
	 * @brief Default destructor.
	 */
	virtual ~TemporalCache(){
		history.clear();
	}

	/**
	 * @brief Insert a new time stamped data into the cache.
	 *
	 * The cache makes sure that all values are sorted by time stamps in descending order.
	 * That means "the older the closer to the end".
	 * Insertions will cause deletions of old values that are beyond the time window.
	 * The window is defined by the latest time stamp and the latest time stamp in the cache
	 * minus the maxHistoryDuration (cache size).
	 *
	 * It is possible that the freshly inserted data will be immediately deleted in case that
	 * it is too outdated.
	 *
	 * As long as deleteOutdatedData() is not called externally (which is possible) there will be always
	 * one element in the cache after the first insertion. However the entry might be rather old.
	 *
	 * It is possible to keep the temporal cache "aligned" with the current system time if
	 * deleteOutdatedData() is triggered periodically with the current system time. In this
	 * case the cache might get flushed completely.
	 *
	 * @param newData The new datum to be inserted. The actual type is defined by the template parameter.
	 * @param timeStamp The accompanying time stamp. It will define the position.
	 * @return False if an entry with this time stamp already exists. In this case data will not be inserted.
	 *         Otherwise true.
	 */
    bool insertData(T newData, TimeStamp timeStamp) {

    	/* history policy: descending order of time stamps (the older the closer to the end - like humans...)
    	 *  latest              oldest
    	 *   |-------------------|
    	 *  begin               end
    	 */

    	historyIterator = history.begin();
    	if (historyIterator != history.end()) {
    		if(timeStamp < (historyIterator->second - maxHistoryDuration)) {
    			LOG(WARNING) << "Can not insert data. An entry at time stamp " << std::fixed << timeStamp.getSeconds() << "[s] is older than the cache limit of "
    					<< std::fixed << (historyIterator->second - maxHistoryDuration).getSeconds() << "[s]. Skipping it.";
    			return false;
    		}
    	}

    	/* insert new data at its correct place in time */
    	while(historyIterator != history.end()) { // loop over history
    		if (historyIterator->second <= timeStamp) {
    			if(historyIterator->second == timeStamp) {
    				LOG(WARNING) << "Can not insert data. An entry at time stamp " << std::fixed << timeStamp.getSeconds() << "[s] exists already. Skipping it.";
    				return false;
    			}
    			break;
    		}
    		historyIterator++;
    	}
    	history.insert(historyIterator, std::make_pair(newData, timeStamp)); // fit into correct temporal place


    	/*
    	 * Clean up outdated data.
    	 * In this case the temporal reference is deduced from the stored data and not
    	 * from the current (real) time.
    	 */
    	historyIterator = history.begin(); // we already know that there is already one element...
    	TimeStamp latestTimeStamp = history.begin()->second;
    	deleteOutdatedData(latestTimeStamp);

    	return true;
    }

    /**
     * @brief Retrieve data from the cache given a time stamp.
     *
     * The "best fitting" cache entry will be returned. That means the entry whose time
     * stamp is closest will be picked. Even if the query reaches beyond the cache limits.
     * I.e. a query older then the oldest entry will return the oldest entry in the cache.
     * Queries that are newer then the latest entry will return the latest one. There is
     * no interpolation of data.
     *
     * To check how far a query is beyond the cache limits, use the getLatestTimeStamp() or
     * getOldestTimeStamp() functions.
     *
     * @param timeStamp Based on this time stamp a value will be returned.
     * @return Returns the best fitting cache entry. The actual type is defined by the template parameter.
     */
    T getData(TimeStamp timeStamp) {
    	//return getData(timeStamp, &getPrecedingData); // getPrecedingData is the _default_ policy here
    	return getData(timeStamp, &getClosestData); // getClosestData is the _default_ policy here
    }

    /**
     * @brief Retrieve data from the cache given a time stamp and a particular access policy.
     * @param timeStamp Based on this time stamp a value will be returned.
     * @param accessPolicy Method on _how_ to retrieve a certain value. Examples are getPrecedingData and getClosestData
     * @return
     */
    template <typename CacheAccessFunctor>
    T getData(TimeStamp timeStamp, CacheAccessFunctor accessPolicy) {
    	typename Cache::iterator closestTransform = accessPolicy(timeStamp, this->history);
    	if(closestTransform == history.end()) {
    		LOG(WARNING) << "TemporalCache is empty. Cannot find data for time stamp at "  << timeStamp.getSeconds() << " [s]";
    		return returnNullData();
    	}
    	return closestTransform->first;
    }

    /**
     * @brief Get a read-only iterator in descending order at the beginning of the cache (latest).
     * @return Iterator
     */
    typename std::vector<std::pair<T, TimeStamp> >::const_iterator begin() {
    	return history.begin();
    }

    /**
     * @brief Get a read-only iterator in descending order at the end of the cache (oldest).
     * @return Iterator
     */
    typename std::vector<std::pair<T, TimeStamp> >::const_iterator end() {
    	return history.end();
    }

    /**
     * @brief Get a read-only iterator in ascending order at the beginning of the cache (oldest).
     * This is probably the iterator that will be used in most applications.
     * @return Iterator
     */
    typename std::vector<std::pair<T, TimeStamp> >::const_reverse_iterator rbegin() {
    	return history.rbegin();
    }

    /**
     * @brief Get a read-only iterator in ascending order at the beginning of the cache (latest).
     * This is probably the iterator that will be used in most applications.
     * @return Iterator
     */
    typename std::vector<std::pair<T, TimeStamp> >::const_reverse_iterator rend() {
    	return history.rend();
    }


    /**
     * @brief Getter for maxHistoryDuration.
     */
    TimeStamp getMaxHistoryDuration() {
    	return maxHistoryDuration;
    }

    /**
     * @brief Setter for maxHistoryDuration.
     *
     * Negative values for time will be ignored.
     */
    void setMaxHistoryDuration(TimeStamp maxHistoryDuration) {
    	if (maxHistoryDuration >= TimeStamp(0.0, Units::Second)) {
    		this->maxHistoryDuration = maxHistoryDuration;
    	} else {
    		LOG(ERROR) << "Cannot set negative duration of "<< maxHistoryDuration.getSeconds() <<"[s] for the temporal cache. Ignoring it.";
    	}
    }

    /**
     * @brief Returns the number of elements that are stored in the history cache.
     * Rename to size?
     */
    unsigned int size() {
    	return static_cast<unsigned int>(history.size());
    }

    /**
     * @brief Get the latest time stamp that is in the history cache.
     * @return The latest time stamp or 0.0 in case of an empty history cache.
     */
    TimeStamp getLatestTimeStamp() {
    	typename std::vector<std::pair<T, TimeStamp> >::iterator historyIterator = history.begin();
    	if(historyIterator == history.end()) {
    		LOG(WARNING) << "The TemporalCache is empty. Returning TimeStamp(0.0) instead.";
    		return TimeStamp(0.0);
    	}
    	return history.front().second;
    }

    /**
     * @brief Get the oldest time stamp that is in the history cache.
     * @return The oldest time stamp or 0.0 in case of an empty history cache.
     */
    TimeStamp getOldestTimeStamp() {
    	typename std::vector<std::pair<T, TimeStamp> >::iterator historyIterator = history.begin();
    	if(historyIterator == history.end()) {
    		LOG(WARNING) << "The TemporalCache is empty. Returning TimeStamp(0.0) instead.";
    		return TimeStamp(0.0);
    	}
    	return history.back().second;
    }

    /**
     * @brief Completely flush the cache.
     */
    void clear() {
    	history.clear();
    }

    /**
     * @brief Delete all data from the history/cache that is older than the latestTimeStamp minus the duration of maxHistoryDuration.
     * @param latestTimeStamp The time stamp the defines the "current" time. To this stamp the maximum duration will relate.
     *        Please note that in case that you do not pass the timeStamp from the history cache via getLatestTimeStamp() the complete history might
     *        get erased! Use the function outside the scope of this class with care.
     */
    void deleteOutdatedData(TimeStamp latestTimeStamp) {
    	/*
    	 * delete all data where the duration (delta between latestTime and stored) exceeds
    	 * the defined maximum history duration
    	 */
    	while(!history.empty() && (history.back().second + maxHistoryDuration < latestTimeStamp)) {
    		history.pop_back();
    	}
    }

    /**
     * @brief The temporal closest cache entry will be returned given a certain time stamp.
     *
     * That means the entry whose time stamp is closest will be picked. Even if the query
     * reaches beyond the cache limits. I.e. a query older then the oldest entry will return
     * the oldest entry in the cache.
     * Queries that are newer then the latest entry will return the latest one. There is
     * no interpolation of data.
     *
     * @param timeStamp Time stamp to which the temporal closest data shall be found.
     * @param cache The cache to be inspected.
     * @return Iterator to the data with closest time stamp. In case timeStamp is exactly in between
     *         two times stamps of the cache the latest of both will be taken.
     *
     */
    static typename Cache::iterator getClosestData(TimeStamp timeStamp, Cache& cache) {

    	typename Cache::iterator resultIterator;
    	typename Cache::iterator previousIterator; //remember: values have a descending order
    	resultIterator = cache.begin();

    	if(cache.size() == 1) { // special case for first element -> just return it
    		return resultIterator;
    	}

    	while(resultIterator != cache.end()) { // loop over history
    		if (timeStamp >= resultIterator->second) {
    			if(resultIterator != cache.begin()) { // i.e. a previous element exists => compare which is actually the closest
    				previousIterator = resultIterator - 1;
    				if ( (previousIterator->second - timeStamp) <= (timeStamp - resultIterator->second) ) {
    					return previousIterator;
    				} else {
    					return resultIterator;
    				}
    			} else {
    				return resultIterator;
    			}
    		}
    		resultIterator++;
    	}

    	/*
    	 * We might reach this line when timeStamp is older than the oldest element in the history.
    	 * In that case we want to return the last/oldest element.
    	 */
    	if(cache.size() > 1) {
    		resultIterator = cache.end() - 1;
    		assert(timeStamp < resultIterator->second); // just to be sure...
    	}
    	return resultIterator;
    }

    /**
     * @brief The temporal closest but still preceding cache entry will be returned
     *        given a certain time stamp.
     *
     * This function is similar to getClosestData but there is a subtitle difference.
     * The entry whose time stamp is closest but _not_ newer will be picked.
     *
     * A query older then the oldest entry will return the oldest entry in the cache.
     * Queries that are newer then the latest entry will return the latest one. There is
     * no interpolation of the data.
     *
     * @param timeStamp Time stamp to which the temporal closest data shall be found.
     * @param cache The cache to be inspected.
     * @return Iterator to the data with closest time stamp. In case timeStamp is exactly in between
     *         two times stamps of the cache the latest of both will be taken.
     */
    static typename Cache::iterator getPrecedingData(TimeStamp timeStamp, Cache& cache) {

     	typename Cache::iterator resultIterator;
     	typename Cache::iterator previousIterator; //remember: values have a descending order
     	resultIterator = cache.begin();

     	if(cache.size() == 1) { // special case for first element -> just return it
     		return resultIterator;
     	}

     	while(resultIterator != cache.end()) { // loop over history
     		if (timeStamp >= resultIterator->second) {
     				return resultIterator;
     		}
     		resultIterator++;
     	}

     	/*
     	 * We might reach this line when timeStamp is older than the oldest element in the history.
     	 * In that case we want to return the last/oldest element.
     	 */
     	if(cache.size() > 1) {
     		resultIterator = cache.end() - 1;
     		assert(timeStamp <= resultIterator->second); // just to be sure...
     	}
     	return resultIterator;
     }

    T returnNullData() {
    	return 0;
    }

    /**
     * @brief Print all entries of the cache.
     * Assumes pointers to be inserted as elements.
     * @return Preformated string with complete cache.
     */
    std::string cacheToString() {
    	std::stringstream result;
    	result.str("");

    	historyIterator = history.begin();
    	while(historyIterator != history.end()) { // loop over history
    		result << "| " << std::setprecision(15) <<historyIterator->second.getSeconds() << " |" << std::endl
    				<< *historyIterator->first
    				<< std::endl;
    		historyIterator++;
    	}

    	return result.str();
    }

protected:

    /// Data cache. Each data T has an associated time stamp.
    Cache history;

    /// Iterator for the cache entries.
    typename Cache::iterator historyIterator;

    /// Size of the cache.
    Duration maxHistoryDuration;

};

}

}

#endif /* BRICS_3D_TEMPORALCACHE_H_ */

/* EOF */
