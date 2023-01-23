// Implementation of ViterbiCodec.
//
// Author: Min Xu <xukmin@gmail.com>
// Date: 01/30/2015

#include "viterbi.hpp"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <limits.h>
#include <string>
#include <utility>
#include <vector>

namespace ex2 {
  namespace mac {

    int ReverseBits(int num_bits, int input)
    {
      assert(input < (1 << num_bits));
      int output = 0;
      while (num_bits-- > 0) {
        output = (output << 1) + (input & 1);
        input >>= 1;
      }
      return output;
    }

    ViterbiCodec::ViterbiCodec(int constraint, const std::vector<int>& polynomials)
    : _constraint(constraint)
    , _poly(polynomials)
    {
      assert(!_poly.empty());
      for (unsigned int i = 0; i < _poly.size(); i++) {
        assert(_poly[i] > 0);
        assert(_poly[i] < (1 << _constraint));
      }

      initPrecomputedShiftRegOutputs();

      // temp variables to save allocation in loops
//      _temp_path_metrics = new std::vector<uint8_t>(1 << (_constraint - 1));
      _temp_path_metrics = new uint8_t[(1 << (_constraint - 1))];
      _temp_trellis_column = new std::vector<uint8_t>(1 << (_constraint - 1));
      assert(_temp_path_metrics != NULL);
      assert(_temp_trellis_column != NULL);

    }

    ViterbiCodec::~ViterbiCodec ()
    {
      if (_temp_path_metrics) {
        delete [] _temp_path_metrics;
      }
      if (_temp_trellis_column) {
        delete _temp_trellis_column;
      }

      freePrecomputedShiftRegOutputs();
    }

    int ViterbiCodec::_next_state(int current_state, int input) const
    {
      return (current_state >> 1) | (input << (_constraint - 2));
    }

    ViterbiCodec::bitarr_t ViterbiCodec::encode(const bitarr_t& bits) const
    {
      bitarr_t encoded;
      int state = 0;
      int rowIndex;

      // Encode the message bits.
      for (unsigned int i = 0; i < bits.size(); i++) {
        // Calculate the current row in the precomputed shift register output
        // matrix based on the current state and the input bit
        rowIndex = state | (bits[i] << (_constraint - 1));
        // The resulting encoded bits are in the column of the precomputed
        // shift register output matrix. Save them all in the encoded array
        for (unsigned int j = 0; j < k_precomputedShiftRegOutputsCols; j++) {
          encoded.push_back(m_precomputedShiftRegOutputs[rowIndex][j]);
        }
        state = _next_state(state, bits[i]);
      }

      return encoded;
    }

    std::vector<uint8_t> ViterbiCodec::encodePacked(const std::vector<uint8_t>& bits) const
    {
      std::vector<uint8_t> encoded;
      int state = 0;
      uint8_t t = 0;
      uint8_t bit = 0;
      uint8_t encodedBits = 0;
      uint8_t encodedBitCount = 0;
      int rowIndex;

      // Encode the message bits.
      for (unsigned int i = 0; i < bits.size(); i++) {
        t = bits[i];
        for (int b = 7; b >= 0; b--) {
          bit = (t >> b) & 0x01;
          // Calculate the current row in the precomputed shift register output
          // matrix based on the current state and the input bit
          rowIndex = state | (bit << (_constraint - 1));
          // The resulting encoded bits are in the column of the precomputed
          // shift register output matrix.
          //
          // Save each element of the column in the encodedBits byte until a
          // full byte is available, then save that in the encoded array.
          for (unsigned int j = 0; j < k_precomputedShiftRegOutputsCols; j++) {
            encodedBits <<= 1;
            encodedBits = encodedBits | m_precomputedShiftRegOutputs[rowIndex][j];
            encodedBitCount++;
            if (encodedBitCount >= 8) {
              encoded.push_back(encodedBits);
              encodedBitCount = 0;
              encodedBits = 0;
            }
          }

          state = _next_state(state, bit);
        } // for each bit in a message byte
      } // for all message bytes

      // check if the number of encoded bits is not an integral multiple of 8
      if (encodedBitCount != 0) {
        encodedBits <<= (8 - encodedBitCount);
        encoded.push_back(encodedBits);
        encodedBitCount = 0;
        encodedBits = 0;
      }

      return encoded;
    }

    void ViterbiCodec::initPrecomputedShiftRegOutputs()
    {
      k_precomputedShiftRegOutputsRows = (1 << _constraint);
      k_precomputedShiftRegOutputsCols = _poly.size();

      // allocate the storage
      m_precomputedShiftRegOutputs = new uint8_t*[k_precomputedShiftRegOutputsRows];
      for (unsigned int i = 0; i < k_precomputedShiftRegOutputsRows; i++) {
        m_precomputedShiftRegOutputs[i] = new uint8_t[k_precomputedShiftRegOutputsCols];
      }
      for (unsigned int i = 0; i < k_precomputedShiftRegOutputsRows; i++) {
        for (unsigned int j = 0; j < k_precomputedShiftRegOutputsCols; j++) {
          // Reverse polynomial bits to make the convolution code simpler.
          int polynomial = ReverseBits(_constraint, _poly[j]);
          int input = i;
          int output = 0;
          for (int k = 0; k < _constraint; k++) {
            output ^= (input & 1) & (polynomial & 1);
            polynomial >>= 1;
            input >>= 1;
          }
          m_precomputedShiftRegOutputs[i][j] = output;
        }
      }
    }

    void ViterbiCodec::freePrecomputedShiftRegOutputs()
    {
      if (m_precomputedShiftRegOutputs) {
        for (unsigned int i = 0; i < k_precomputedShiftRegOutputsRows; i++) {
          delete [] m_precomputedShiftRegOutputs[i];
        }
        delete [] m_precomputedShiftRegOutputs;
      }
    }

    int ViterbiCodec::_branch_metric(const uint8_t* bits, uint8_t numBits, int source_state, int target_state) const
    {
      int index = source_state | ((target_state >> (_constraint - 2)) << (_constraint - 1));

      // Calculate the Hamming distance
      int distance = 0;
      for (unsigned int i = 0; i < numBits; i++) {
        distance += (bits[i] != (m_precomputedShiftRegOutputs[index][i]));
      }
      return distance;
    }

//    void ViterbiCodec::_path_metric(const uint8_t* bits, uint8_t numBits,
//      const std::vector<uint8_t>& prev_path_metrics, int state,
//      uint8_t *newPathMetric, uint8_t *previousState) const
    void ViterbiCodec::_path_metric(const uint8_t* bits, uint8_t numBits,
      const uint8_t *prev_path_metrics, int state,
      uint8_t *newPathMetric, uint8_t *previousState) const
    {
      int s = (state & ((1 << (_constraint - 2)) - 1)) << 1;
      int source_state1 = s | 0;
      int source_state2 = s | 1;

      int pm1 = prev_path_metrics[source_state1];
      if (pm1 < INT_MAX) {
        pm1 += _branch_metric(bits, numBits, source_state1, state);
      }
      int pm2 = prev_path_metrics[source_state2];
      if (pm2 < INT_MAX) {
        pm2 += _branch_metric(bits, numBits, source_state2, state);
      }

      if (pm1 <= pm2) {
        *newPathMetric = pm1;
        *previousState = source_state1;
      }
      else {
        *newPathMetric = pm2;
        *previousState = source_state2;
      }
    }

//    void ViterbiCodec::_update_path_metrics(const uint8_t* bits, uint8_t numBits, std::vector<uint8_t>& path_metrics,
//      Trellis& trellis) const
    void ViterbiCodec::_update_path_metrics(const uint8_t* bits, uint8_t numBits, uint8_t *path_metrics, uint16_t path_metrics_length,
      Trellis& trellis) const
    {
      uint8_t newPathMetric;
      uint8_t previousState;
      for (unsigned int i = 0; i < path_metrics_length; i++) {
        _path_metric(bits, numBits, path_metrics, i, &newPathMetric, &previousState);
        _temp_path_metrics[i] = newPathMetric;
        (*_temp_trellis_column)[i] = previousState;
      }

//      path_metrics = (*_temp_path_metrics);
      for (unsigned int i = 0; i < path_metrics_length; i++) {
        path_metrics[i] = _temp_path_metrics[i];
      }
      trellis.push_back((*_temp_trellis_column));
    }

    ViterbiCodec::bitarr_t ViterbiCodec::decode(const bitarr_t& bits) const
    {
      // Compute path metrics and generate trellis.
      Trellis trellis;
//      std::vector<uint8_t> path_metrics(1 << (_constraint - 1), UCHAR_MAX);
//      path_metrics.front() = 0;
      uint16_t path_metrics_length = (1 << (_constraint - 1));
      uint8_t *path_metrics = new uint8_t[path_metrics_length];
      for (unsigned int i = 0; i < path_metrics_length; i++) {
        path_metrics[i] = UCHAR_MAX;
      }
      path_metrics[0] = 0;
      const unsigned int poly_len = _poly.size();
      const uint8_t* encodedBits;
      // @note we never need to worry that stepping throught the encodedBits array
      // we will end up with too few bits in the last iteration because the
      // @p encode and @p encodePacked methods will always produce a multiple of
      // @p poly_len bits
      encodedBits = &bits[0];
      for (unsigned int i = 0; i < bits.size(); i += poly_len) {
        _update_path_metrics(encodedBits, poly_len, path_metrics, path_metrics_length, trellis);
        encodedBits += poly_len;
      }

      // Traceback.
      bitarr_t decoded;
//      int state = std::min_element(path_metrics.begin(), path_metrics.end()) - path_metrics.begin();
      // Find the first index of the minimum element in the path_metrics
      int state = 0;
      for (unsigned int i = 0; i < path_metrics_length; i++) {
        if (path_metrics[i] < path_metrics[state]) {
          state = i;
        }
      }
      for (int i = trellis.size() - 1; i >= 0; i--) {
        decoded.push_back(state >> (_constraint - 2));
        state = trellis[i][state];
      }
      std::reverse(decoded.begin(), decoded.end());

      delete [] path_metrics;

      return decoded;
    } // decode

    ViterbiCodec::bitarr_t ViterbiCodec::decodeTruncated(const bitarr_t& bits) const
    {

      bitarr_t decoded;
      decoded.resize(bits.size()/2,0);

      unsigned int truncLength = 0;

      // Compute path metrics and generate trellis.
      Trellis trellis;
      trellis.reserve(_constraint*5);

//      std::vector<uint8_t> path_metrics(1 << (_constraint - 1), UCHAR_MAX);
//      path_metrics.front() = 0;
      uint16_t path_metrics_length = (1 << (_constraint - 1));
      uint8_t *path_metrics = new uint8_t[path_metrics_length];
      for (unsigned int i = 0; i < path_metrics_length; i++) {
        path_metrics[i] = UCHAR_MAX;
      }
      path_metrics[0] = 0;

      const unsigned int poly_len = _poly.size();
      const uint8_t* encodedBits;
      // @note we never need to worry that stepping throught the encodedBits array
      // we will end up with too few bits in the last iteration because the
      // @p encode and @p encodePacked methods will always produce a multiple of
      // @p poly_len bits
      encodedBits = &bits[0];
      for (unsigned int i = 0; i < bits.size(); i += poly_len) {
        _update_path_metrics(encodedBits, poly_len, path_metrics, path_metrics_length, trellis);
        if (trellis.size() >= trellis.capacity()) {
//          int state = std::min_element(path_metrics.begin(), path_metrics.end()) - path_metrics.begin();
          // Find the first index of the minimum element in the path_metrics
          int state = 0;
          for (unsigned int i = 0; i < path_metrics_length; i++) {
            if (path_metrics[i] < path_metrics[state]) {
              state = i;
            }
          }
          for (int i = trellis.size() - 1; i >= 0; i--) {
            decoded[i + truncLength] = (state >> (_constraint - 2));
            state = trellis[i][state];
            trellis.pop_back();
          }
          truncLength += (_constraint*5);
        }
        encodedBits += poly_len;

      }
      if (trellis.size() > 0) {
//        int state = std::min_element(path_metrics.begin(), path_metrics.end()) - path_metrics.begin();
        // Find the first index of the minimum element in the path_metrics
        int state = 0;
        for (unsigned int i = 0; i < path_metrics_length; i++) {
          if (path_metrics[i] < path_metrics[state]) {
            state = i;
          }
        }
        for (int i = trellis.size() -1; i >= 0; i--) {
          decoded[i + truncLength] = (state >> (_constraint - 2));
          state = trellis[i][state];
          trellis.pop_back();
        }
      }

      delete [] path_metrics;

      return decoded;
    } // decodeTruncated

  } /* namespace mac */
} /* namespace ex2 */

