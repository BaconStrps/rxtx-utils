#pragma once
#include <chrono>
#include <concepts>
#include <csics/queue/SPSCQueue.hpp>
#include <memory>

#ifdef USE_UHD
#include <uhd/types/device_addr.hpp>
#endif

namespace csics::radio {

/** @brief Configuration parameters for the radio receiver. */
struct RadioConfiguration {
    // Sample rate in Hz.
    double sample_rate = 1e6;
    // Center frequency in Hz.
    double center_frequency = 2.437e9;
    // Gain in dB.
    double gain = 0.0;
    // Channel bandwidth in Hz.
    double channel_bandwidth = 1e6;
};

struct RadioDeviceInfo {
    struct {
        double min;
        double max;
    } frequency_range;
    struct {
        double min;
        double max;
    } sample_rate_range;
    double max_gain;
};

enum class DeviceType {
    DEFAULT,
#ifdef USE_UHD
    USRP,
#endif
};

#ifdef USE_UHD
struct UsrpArgs {
    uhd::device_addr_t device_addr;

    RadioDeviceArgs to_radio_device_args() {
        RadioDeviceArgs args;
        args.device_type = DeviceType::USRP;
        args.args.usrp_args = *this;
        return args;
    }
};
#endif

struct RadioDeviceArgs;

template <typename T>
concept RadioDeviceArgsConvertible = requires(T t) {
    { t.to_radio_device_args() } -> std::same_as<RadioDeviceArgs>;
};

struct RadioDeviceArgs {
    DeviceType device_type;
    union {
        struct {} default_args;
#ifdef USE_UHD
        UsrpArgs usrp_args;
#endif
    } args;

    template <RadioDeviceArgsConvertible T>
    static RadioDeviceArgs from_device_args(T& args) {
        return args.to_radio_device_args();
    }

    template <RadioDeviceArgsConvertible T>
    RadioDeviceArgs(T& args) : RadioDeviceArgs(from_device_args(args)) {}
    RadioDeviceArgs() : device_type(DeviceType::DEFAULT), args{} {}

    RadioDeviceArgs to_radio_device_args() { return *this; }
};

/**
 * @brief Defines the host-side data type for IQ samples.
 */
enum class StreamDataType {
    IC8,   // 8-bit signed integer complex, IQ interleaved
    IC16,  // 16-bit signed integer complex, IQ interleaved
    FC32,  // 32-bit float complex, IQ interleaved
};

struct SampleLength {
    enum class Type {
        NUM_SAMPLES,
        DURATION,
    } type;
    union {
        std::size_t num_samples = 0;
        std::chrono::nanoseconds duration;
    };
};

struct StreamConfiguration {
    StreamDataType data_type = StreamDataType::FC32;
    SampleLength sample_length = {SampleLength::Type::NUM_SAMPLES, {1024}};
};


/** @brief Abstract base class for a radio receiver.
 * Abstracts over different radio hardware implementations.
 * Provides a common interface for opening the receiver.
 * Represents a single channel.
 * The returned queue from start_stream() will provide raw IQ samples.
 * The queue is owned by the RadioRx implementation and will be valid until the
 * radio is destroyed.
 */
class RadioRx {
   public:
    struct StartStatus {
        enum class Code {
            SUCCESS,
            ALREADY_STARTED,
            HARDWARE_FAILURE,
            CONFIGURATION_ERROR,
        } code;
        csics::queue::SPSCQueue* queue = nullptr;

        operator bool() const noexcept {
            return code == Code::SUCCESS && queue != nullptr;
        }
    };

    RadioRx() = delete;
    virtual ~RadioRx() = default;

    /** 
     * @brief Starts the radio stream.
     * @param stream_config Configuration for the stream.
     * @return StartStatus indicating success or failure, and the queue for receiving samples.
     */ 
    virtual StartStatus start_stream(const StreamConfiguration* const stream_config) noexcept = 0;

    /** 
     * @brief Stops the radio stream.
     * 
     * Stops the radio stream if it is currently streaming.
     * If the stream is not active, this function has no effect.
     * If the stream is active, no more samples will be produced after this call.
     * The queue still remains valid until the RadioRx object is destroyed.
     */
    virtual void stop_stream() noexcept = 0;

    virtual bool is_streaming() const noexcept = 0;

    virtual double get_sample_rate() const noexcept = 0;
    virtual void set_sample_rate(double rate) noexcept = 0;
    virtual double get_max_sample_rate() const noexcept = 0;

    virtual double get_center_frequency() const noexcept = 0;
    virtual void set_center_frequency(double freq) noexcept = 0;

    virtual double get_gain() const noexcept = 0;
    virtual void set_gain(double gain) noexcept = 0;

    virtual double get_channel_bandwidth() const noexcept = 0;
    virtual void set_channel_bandwidth(double bandwidth) noexcept = 0;

    virtual RadioConfiguration get_configuration() const noexcept = 0;
    virtual void set_configuration(
        const RadioConfiguration& config) noexcept = 0;
    virtual RadioDeviceInfo get_device_info() const noexcept = 0;

    template <RadioDeviceArgsConvertible T>
    static std::unique_ptr<RadioRx> create_radio_rx(
        T device_args, const RadioConfiguration& config);
};

};  // namespace csics::radio
