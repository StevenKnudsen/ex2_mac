/*!
 * @file FEC.hpp
 * @author StevenKnudsen
 * @date June 21, 2021
 *
 * @details A Forward Error Correction factory that creates instances of various
 * FEC codecs.
 *
 * @copyright AlbertaSat 2021
 *
 * @license
 * This software may not be modified or distributed in any form, except as described in the LICENSE file.
 */

#ifndef EX2_MAC_ERROR_CONTROL_FEC_H_
#define EX2_MAC_ERROR_CONTROL_FEC_H_

#include <cstdint>
#include <string>
#include <vector>
#include <stdexcept>

#include "error_correction.hpp"

namespace ex2 {
  namespace mac {


    class FECException: public std::runtime_error {

    public:
      FECException(const std::string& message);
    };

    /*!
     * @brief Define a forward error correction scheme.
     */
    class FEC {
    public:
      static FEC *makeFECCodec(ErrorCorrection::ErrorCorrectionScheme ecScheme);

      FEC(ErrorCorrection::ErrorCorrectionScheme ecScheme);

      virtual ~FEC() {}

      /*!
       * @brief A virtual function to encode a payload using the FEC scheme
       *
       * @param[in] payload The payload to encode
       * @return The encoded payload
       */
      virtual std::vector<uint8_t> encode(const std::vector<uint8_t>& payload) = 0;

      /*!
       * @brief A virtual function to decode a payload using the FEC scheme
       *
       * @todo It may be better to not have @p encodedPayload as const. What if
       * it needs to be manipulated? After all, there is no contract saying it's
       * not destroyed afrter decoding
       *
       * @param[in] encodedPayload The encoded payload
       * @param[in] snrEstimate An estimate of the SNR for FEC schemes that need it.
       * @param[out] decodedPayload The resulting decoded payload
       * @return The number of bit errors from the decoding process
       */
      virtual uint32_t decode(std::vector<uint8_t>& encodedPayload, float snrEstimate,
        std::vector<uint8_t>& decodedPayload) = 0;

    private:
      ErrorCorrection::ErrorCorrectionScheme m_ecScheme;
    };

  } /* namespace mac */
} /* namespace ex2 */

#endif /* EX2_MAC_ERROR_CONTROL_FEC_H_ */
