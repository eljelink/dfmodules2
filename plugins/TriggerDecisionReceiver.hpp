/**
 * @file TriggerDecisionReceiver.hpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef DFMODULES_PLUGINS_TRIGGERDECISIOMRECEIVER_HPP_
#define DFMODULES_PLUGINS_TRIGGERDECISIONRECEIVER_HPP_

#include "dfmessages/TriggerDecision.hpp"

#include "appfwk/DAQModule.hpp"
#include "appfwk/DAQSink.hpp"
#include "ipm/Receiver.hpp"

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace dunedaq {
namespace dfmodules {

/**
 * @brief TriggerDecisionReceiver receives triggerdecisions then dispatches them to the appropriate queue
 */
class TriggerDecisionReceiver : public dunedaq::appfwk::DAQModule
{
public:
  /**
   * @brief TriggerDecisionReceiver Constructor
   * @param name Instance name for this TriggerDecisionReceiver instance
   */
  explicit TriggerDecisionReceiver(const std::string& name);

  TriggerDecisionReceiver(const TriggerDecisionReceiver&) = delete;            ///< TriggerDecisionReceiver is not copy-constructible
  TriggerDecisionReceiver& operator=(const TriggerDecisionReceiver&) = delete; ///< TriggerDecisionReceiver is not copy-assignable
  TriggerDecisionReceiver(TriggerDecisionReceiver&&) = delete;                 ///< TriggerDecisionReceiver is not move-constructible
  TriggerDecisionReceiver& operator=(TriggerDecisionReceiver&&) = delete;      ///< TriggerDecisionReceiver is not move-assignable

  void init(const data_t&) override;

private:
  // Commands
  void do_conf(const data_t&);
  void do_start(const data_t&);
  void do_stop(const data_t&);

  void get_info(opmonlib::InfoCollector& ci, int level) override;

  void dispatch_triggerdecision(ipm::Receiver::Response message);

  // Configuration
  std::chrono::milliseconds m_queue_timeout;
  dunedaq::dataformats::run_number_t m_run_number;
  std::string m_connection_name;

  // Queue(s)
  using triggerdecisionsink_t = dunedaq::appfwk::DAQSink<dfmessages::TriggerDecision>;
  std::unique_ptr<triggerdecisionsink_t> m_triggerdecision_output_queue;

  size_t m_received_triggerdecisions{ 0 };
};
} // namespace dfmodules
} // namespace dunedaq

#endif // DFMODULES_PLUGINS_TRIGGERDECISIONRECEIVER_HPP_
