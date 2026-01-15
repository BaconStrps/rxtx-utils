#pragma once
#include <csics/radio/RadioRx.hpp>
#include <uhd/usrp/multi_usrp.hpp>

namespace csics::radio {
class USRPRadioRx : public RadioRx {
   public:
    explicit USRPRadioRx(const RadioDeviceArgs& device_args);
    ~USRPRadioRx() override;
    StartStatus start_stream(
        const StreamConfiguration* const stream_config) noexcept override;

    void stop_stream() noexcept override;

    bool is_streaming() const noexcept override;

    double get_sample_rate() const noexcept override;
    void set_sample_rate(double rate) noexcept override;
    double get_max_sample_rate() const noexcept override;

    double get_center_frequency() const noexcept override;
    void set_center_frequency(double freq) noexcept override;

    double get_gain() const noexcept override;
    void set_gain(double gain) noexcept override;

    double get_channel_bandwidth() const noexcept override;
    void set_channel_bandwidth(double bandwidth) noexcept override;

    RadioConfiguration get_configuration() const noexcept override;
    void set_configuration(const RadioConfiguration& config) noexcept override;
    RadioDeviceInfo get_device_info() const noexcept override;
   private:
    csics::queue::SPSCQueue queue_;
    uhd::usrp::multi_usrp::sptr usrp_;
    uhd::rx_streamer::sptr rx_streamer_;
};
};  // namespace csics::radio
