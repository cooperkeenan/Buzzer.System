#ifndef I_TIME_PROVIDER_H
#define I_TIME_PROVIDER_H

/**
 * @brief Interface for wall-clock time
 *
 * Abstracted so unlock freshness checks can be tested without waiting
 * on a real network time sync.
 */
class ITimeProvider {
public:
    virtual ~ITimeProvider() = default;

    /**
     * @brief Acquire the current time from the network
     * @return true if the clock was set, false otherwise
     */
    virtual bool synchronize() = 0;

    /**
     * @brief Current time as epoch seconds
     * @return Seconds since the Unix epoch, or 0 if never synchronized
     */
    virtual unsigned long epochSeconds() const = 0;

    /**
     * @brief Whether the clock has been set
     * @return true if the time is trustworthy, false otherwise
     */
    virtual bool isSynchronized() const = 0;
};

#endif
