#pragma once

#include <chrono>

namespace ko {

/**
 * Timer to measure the elapsed time.
 */
class Timer {
public:
    /// Define the used clock
    using Clock = std::chrono::high_resolution_clock;

    /**
     * Default constructor for the timer.
     */
    Timer()
      : Timer(0){};

    /**
     * Constructs the timer with a given time limit.
     *
     * @param timeLimitSeconds The given time limit in seconds
     */
    explicit Timer(double timeLimitSeconds)
      : m_timeLimitSeconds(timeLimitSeconds)
      , m_startTime(Clock::now()){};

    /**
     * Returns the elapsed seconds since the start of the timer.
     *
     * @return The elapsed seconds
     */
    [[nodiscard]] double elapsedSeconds() const {
        const auto timeNow = Clock::now();
        const std::chrono::duration<double> duration = timeNow - m_startTime;
        return duration.count();
    }

    /**
     * Returns the remaining seconds.
     *
     * @return The remaining seconds
     */
    [[nodiscard]] double remainingSeconds() const { return timeLimitSeconds() - elapsedSeconds(); }

    /**
     * Returns if the time limit is reached or not.
     *
     * @return True if the time limit is reached, false otherwise
     */
    [[nodiscard]] bool notTimeLimitReached() const { return elapsedSeconds() < m_timeLimitSeconds; }

    /**
     * Returns the given time limit in seconds.
     *
     * @return The time limit in seconds
     */
    [[nodiscard]] const double& timeLimitSeconds() const { return m_timeLimitSeconds; }

private:
    /// The time limit in seconds
    double m_timeLimitSeconds;
    /// The start time point
    Clock::time_point m_startTime;
};

} // namespace ko
