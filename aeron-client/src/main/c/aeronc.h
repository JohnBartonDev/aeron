/*
 * Copyright 2014-2020 Real Logic Limited.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef AERON_C_H
#define AERON_C_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define AERON_NULL_VALUE (-1)

typedef struct aeron_context_stct aeron_context_t;
typedef struct aeron_stct aeron_t;
typedef struct aeron_buffer_claim_stct aeron_buffer_claim_t;
typedef struct aeron_publication_stct aeron_publication_t;
typedef struct aeron_exclusive_publication_stct aeron_exclusive_publication_t;
typedef struct aeron_header_stct aeron_header_t;
typedef struct aeron_subscription_stct aeron_subscription_t;
typedef struct aeron_image_stct aeron_image_t;
typedef struct aeron_counter_stct aeron_counter_t;
typedef struct aeron_log_buffer_stct aeron_log_buffer_t;

typedef struct aeron_counters_reader_stct aeron_counters_reader_t;

typedef struct aeron_client_registering_resource_stct aeron_async_add_publication_t;
typedef struct aeron_client_registering_resource_stct aeron_async_add_exclusive_publication_t;
typedef struct aeron_client_registering_resource_stct aeron_async_add_subscription_t;
typedef struct aeron_client_registering_resource_stct aeron_async_add_counter_t;

typedef struct aeron_image_fragment_assembler_stct aeron_image_fragment_assembler_t;
typedef struct aeron_image_controlled_fragment_assembler_stct aeron_image_controlled_fragment_assembler_t;
typedef struct aeron_fragment_assembler_stct aeron_fragment_assembler_t;
typedef struct aeron_controlled_fragment_assembler_stct aeron_controlled_fragment_assembler_t;

/**
 * Environment variables and functions used for setting values of an aeron_context_t.
 */

/**
 * The top level Aeron directory used for communication between a Media Driver and client.
 */
#define AERON_DIR_ENV_VAR "AERON_DIR"

int aeron_context_set_dir(aeron_context_t *context, const char *value);
const char *aeron_context_get_dir(aeron_context_t *context);

#define AERON_DRIVER_TIMEOUT_ENV_VAR "AERON_DRIVER_TIMEOUT"

int aeron_context_set_driver_timeout_ms(aeron_context_t *context, uint64_t value);
uint64_t aeron_context_get_driver_timeout_ms(aeron_context_t *context);

int aeron_context_set_keepalive_interval_ns(aeron_context_t *context, uint64_t value);
uint64_t aeron_context_get_keepalive_interval_ns(aeron_context_t *context);

#define AERON_CLIENT_RESOURCE_LINGER_DURATION_ENV_VAR "AERON_CLIENT_RESOURCE_LINGER_DURATION"

int aeron_context_set_resource_linger_duration_ns(aeron_context_t *context, uint64_t value);
uint64_t aeron_context_get_resource_linger_duration_ns(aeron_context_t *context);

/**
 * The error handler to be called when an error occurs.
 */
typedef void (*aeron_error_handler_t)(void *clientd, int errcode, const char *message);

int aeron_context_set_error_handler(aeron_context_t *context, aeron_error_handler_t handler, void *clientd);
aeron_error_handler_t aeron_context_get_error_handler(aeron_context_t *context);
void *aeron_context_get_error_handler_clientd(aeron_context_t *context);

/**
 * Function called by aeron_client_t to deliver notification that the media driver has added an aeron_publication_t
 * or aeron_exclusive_publication_t successfully.
 *
 * Implementations should do the minimum work for passing off state to another thread for later processing.
 *
 * @param clientd to be returned in the call
 * @param async associated with the original add publication call
 * @param channel of the publication
 * @param stream_id within the channel of the publication
 * @param session_id of the publication
 * @param correlation_id used by the publication
 */
typedef void (*aeron_on_new_publication_t)(
    void *clientd,
    aeron_async_add_publication_t *async,
    const char *channel,
    int32_t stream_id,
    int32_t session_id,
    int64_t correlation_id);

int aeron_context_set_on_new_publication(aeron_context_t *context, aeron_on_new_publication_t handler, void *clientd);
aeron_on_new_publication_t aeron_context_get_on_new_publication(aeron_context_t *context);
void *aeron_context_get_on_new_publication_clientd(aeron_context_t *context);

int aeron_context_set_on_new_exclusive_publication(
    aeron_context_t *context, aeron_on_new_publication_t handler, void *clientd);
aeron_on_new_publication_t aeron_context_get_on_new_exclusive_publication(aeron_context_t *context);
void *aeron_context_get_on_new_exclusive_publication_clientd(aeron_context_t *context);

/**
 * Function called by aeron_client_t to deliver notification that the media driver has added an aeron_subscription_t
 * successfully.
 *
 * Implementations should do the minimum work for passing off state to another thread for later processing.
 *
 * @param clientd to be returned in the call
 * @param async associated with the original aeron_add_async_subscription call
 * @param channel of the subscirption
 * @param stream_id within the channel of the subscription
 * @param session_id of the subscription
 * @param correlation_id used by the subscription
 */
typedef void (*aeron_on_new_subscription_t)(
    void *clientd,
    aeron_async_add_subscription_t *async,
    const char *channel,
    int32_t stream_id,
    int64_t correlation_id);

int aeron_context_set_on_new_subscription(
    aeron_context_t *context, aeron_on_new_subscription_t handler, void *clientd);
aeron_on_new_subscription_t aeron_context_get_on_new_subscription(aeron_context_t *context);
void *aeron_context_get_on_new_subscription_clientd(aeron_context_t *context);

/**
 * Function called by aeron_client_t to deliver notifications that an aeron_image_t was added.
 *
 * @param clientd to be returned in the call.
 * @param subscription that image is part of.
 * @param image that has become available.
 */
typedef void (*aeron_on_available_image_t)(void *clientd, aeron_subscription_t *subscription, aeron_image_t *image);

/**
 * Function called by aeron_client_t to deliver notifications that an aeron_image_t has been removed from use and
 * should not be used any longer.
 *
 * @param clientd to be returned in the call.
 * @param subscription that image is part of.
 * @param image that has become unavailble.
 */
typedef void (*aeron_on_unavailable_image_t)(void *clientd, aeron_subscription_t *subscription, aeron_image_t *image);

/**
 * Function called by aeron_client_t to deliver notifications that a counter has been added to the driver.
 *
 * @param clientd to be returned in the call.
 * @param counters_reader that holds the counter.
 * @param registration_id of the counter.
 * @param counter_id of the counter.
 */
typedef void (*aeron_on_available_counter_t)(
    void *clientd, aeron_counters_reader_t *counters_reader, int64_t registration_id, int32_t counter_id);

int aeron_context_set_on_available_counter(
    aeron_context_t *context, aeron_on_available_counter_t handler, void *clientd);
aeron_on_available_counter_t aeron_context_get_on_available_counter(aeron_context_t *context);
void *aeron_context_get_on_available_counter_clientd(aeron_context_t *context);

/**
 * Function called by aeron_client_t to deliver notifications that a counter has been removed from the driver.
 *
 * @param clientd to be returned in the call.
 * @param counters_reader that holds the counter.
 * @param registration_id of the counter.
 * @param counter_id of the counter.
 */
typedef void (*aeron_on_unavailable_counter_t)(
    void *clientd, aeron_counters_reader_t *counters_reader, int64_t registration_id, int32_t counter_id);

int aeron_context_set_on_unavailable_counter(
    aeron_context_t *context, aeron_on_unavailable_counter_t handler, void *clientd);
aeron_on_unavailable_counter_t aeron_context_get_on_unavailable_counter(aeron_context_t *context);
void *aeron_context_get_on_unavailable_counter_clientd(aeron_context_t *context);

/**
 * Function called by aeron_client_t to deliver notifications that the client is closing.
 *
 * @param clientd to be returned in the call.
 */
typedef void (*aeron_on_close_client_t)(void *clientd);

int aeron_context_set_on_close_client(
    aeron_context_t *context, aeron_on_close_client_t handler, void *clientd);
aeron_on_close_client_t aeron_context_get_on_close_client(aeron_context_t *context);
void *aeron_context_get_on_close_client_clientd(aeron_context_t *context);

/**
 * Whether to use an invoker to control the conductor agent or spawn a thread.
 */
int aeron_context_set_use_conductor_agent_invoker(aeron_context_t *context, bool value);
bool aeron_context_get_use_conductor_agent_invoker(aeron_context_t *context);

/**
 * Function name to call on start of each agent.
 */
#define AERON_AGENT_ON_START_FUNCTION_ENV_VAR "AERON_AGENT_ON_START_FUNCTION"

typedef void (*aeron_agent_on_start_func_t)(void *state, const char *role_name);

int aeron_context_set_agent_on_start_function(
    aeron_context_t *context, aeron_agent_on_start_func_t value, void *state);
aeron_agent_on_start_func_t aeron_context_get_agent_on_start_function(aeron_context_t *context);
void *aeron_context_get_agent_on_start_state(aeron_context_t *context);

/**
 * Create a aeron_context_t struct and initialize with default values.
 *
 * @param context to create and initialize
 * @return 0 for success and -1 for error.
 */
int aeron_context_init(aeron_context_t **context);

/**
 * Close and delete aeron_context_t struct.
 *
 * @param context to close and delete
 * @return 0 for success and -1 for error.
 */
int aeron_context_close(aeron_context_t *context);

/**
 * Create a aeron_t client struct and initialize from the aeron_context_t struct.
 *
 * The given aeron_context_t struct will be used exclusively by the client. Do not reuse between clients.
 *
 * @param aeron  client to create and initialize.
 * @param context to use for initialization.
 * @return 0 for success and -1 for error.
 */
int aeron_init(aeron_t **client, aeron_context_t *context);

/**
 * Start an aeron_t. This may spawn a thread for the Client Conductor.
 *
 * @param client to start.
 * @return 0 for success and -1 for error.
 */
int aeron_start(aeron_t *client);

/**
 * Call the Conductor main do_work duty cycle once.
 *
 * Client must have been created with use conductor invoker set to true.
 *
 * @param client to call do_work duty cycle on.
 * @return 0 for success and -1 for error.
 */
int aeron_main_do_work(aeron_t *client);

/**
 * Call the Conductor Idle Strategy.
 *
 * @param client to idle.
 * @param work_count to pass to idle strategy.
 */
void aeron_main_idle_strategy(aeron_t *client, int work_count);

/**
 * Close and delete aeron_t struct.
 *
 * @param client to close and delete
 * @return 0 for success and -1 for error.
 */
int aeron_close(aeron_t *client);

/*
 * Aeron API functions
 */

/**
 * Call stream_out to print the counter labels and values.
 *
 * @param client to get the counters from
 * @param stream_out to call for each label and value
 */
void aeron_print_counters(aeron_t *client, void (*stream_out)(const char *));

/**
 * Return the aeron_context_t that is in use by the given client.
 *
 * @param client to return the aeron_context_t for.
 * @return the aeron_context_t for the given client or NULL for an error.
 */
aeron_context_t *aeron_context(aeron_t *client);

/**
 * Return the client id in use by the client.
 *
 * @param client to return the client id for.
 * @return id value or -1 for an error.
 */
int64_t aeron_client_id(aeron_t *client);

/**
 * Return a unique correlation id from the driver.
 *
 * @param client to use to get the id.
 * @return unique correlation id or -1 for an error.
 */
int64_t aeron_next_correlation_id(aeron_t *client);

/**
 * Asynchronously add a publication using the given client and return an object to use to determine when the
 * publication is available.
 *
 * @param async object to use for polling completion.
 * @param client to add the publication to.
 * @param uri for the channel of the publication.
 * @param stream_id for the publication.
 * @return 0 for success or -1 for an error.
 */
int aeron_async_add_publication(
    aeron_async_add_publication_t **async, aeron_t *client, const char *uri, int32_t stream_id);

/**
 * Poll the completion of the aeron_async_add_publication call.
 *
 * @param publication to set if completed successfully.
 * @param async to check for completion.
 * @return 0 for not cmplete (try again), 1 for completed successfully, or -1 for an error.
 */
int aeron_async_add_publication_poll(aeron_publication_t **publication, aeron_async_add_publication_t *async);

/**
 * Asynchronously add an exclusive publication using the given client and return an object to use to determine when the
 * publication is available.
 *
 * @param async object to use for polling completion.
 * @param client to add the publication to.
 * @param uri for the channel of the publication.
 * @param stream_id for the publication.
 * @return 0 for success or -1 for an error.
 */
int aeron_async_add_exclusive_publication(
    aeron_async_add_exclusive_publication_t **async, aeron_t *client, const char *uri, int32_t stream_id);

/**
 * Poll the completion of the aeron_async_add_exclusive_publication call.
 *
 * @param publication to set if completed successfully.
 * @param async to check for completion.
 * @return 0 for not cmplete (try again), 1 for completed successfully, or -1 for an error.
 */
int aeron_async_add_exclusive_publication_poll(
    aeron_exclusive_publication_t **publication, aeron_async_add_exclusive_publication_t *async);

/**
 * Asynchronously add a subscription using the given client and return an object to use to determine when the
 * subscription is available.
 *
 * @param async object to use for polling completion.
 * @param client to add the subscription to.
 * @param uri for the channel of the subscription.
 * @param stream_id for the subscription.
 * @param on_available_image_handler to be called when images become available on the subscription.
 * @param on_available_image_clientd to be passed when images become available on the subscription.
 * @param on_unavailable_image_handler to be called when images go unavailable on the subscription.
 * @param on_available_image_clientd to be pacced when images go unavailable on the subscription.
 * @return 0 for success or -1 for an error.
 */
int aeron_async_add_subscription(
    aeron_async_add_subscription_t **async,
    aeron_t *client,
    const char *uri,
    int32_t stream_id,
    aeron_on_available_image_t on_available_image_handler,
    void *on_available_image_clientd,
    aeron_on_unavailable_image_t on_unavailable_image_handler,
    void *on_unavailable_image_clientd);

/**
 * Poll the completion of the aeron_async_add_subscription call.
 *
 * @param subscription to set if completed successfully.
 * @param async to check for completion.
 * @return 0 for not cmplete (try again), 1 for completed successfully, or -1 for an error.
 */
int aeron_async_add_subscription_poll(aeron_subscription_t **subscription, aeron_async_add_subscription_t *async);

/**
 * Return a reference to the counters reader of the given client.
 *
 * The aeron_counters_reader_t is maintained by the client. And should not be freed.
 *
 * @param client that contains the counters reader.
 * @return aeron_counters_reader_t or NULL for error.
 */
aeron_counters_reader_t *aeron_counters_reader(aeron_t *client);

/**
 * Asynchronously add a counter using the given client and return an object to use to determine when the
 * counter is available.
 *
 * @param async object to use for polling completion.
 * @param client to add the counter to.
 * @param type_id for the counter.
 * @param key_buffer for the counter.
 * @param key_buffer_length for the counter.
 * @param label_buffer for the counter.
 * @param label_buffer_length for the counter.
 * @return 0 for success or -1 for an error.
 */
int aeron_async_add_counter(
    aeron_async_add_counter_t **async,
    aeron_t *client,
    int32_t type_id,
    const uint8_t *key_buffer,
    size_t key_buffer_length,
    const char *label_buffer,
    size_t label_buffer_length);

/**
 * Poll the completion of the aeron_async_add_counter call.
 *
 * @param counter to set if completed successfully.
 * @param async to check for completion.
 * @return 0 for not cmplete (try again), 1 for completed successfully, or -1 for an error.
 */
int aeron_async_add_counter_poll(aeron_counter_t **counter, aeron_async_add_counter_t *async);

typedef struct aeron_on_available_counter_pair_stct
{
    aeron_on_available_counter_t handler;
    void *clientd;
}
aeron_on_available_counter_pair_t;

typedef struct aeron_on_unavailable_counter_pair_stct
{
    aeron_on_unavailable_counter_t handler;
    void *clientd;
}
aeron_on_unavailable_counter_pair_t;

typedef struct aeron_on_close_client_pair_stct
{
    aeron_on_close_client_t handler;
    void *clientd;
}
aeron_on_close_client_pair_t;

/**
 * Add a handler to be called when a new counter becomes available.
 *
 * NOTE: This function blocks until the handler is added by the client conductor thread.
 *
 * @param client for the counter
 * @param pair holding the handler to call and a clientd to pass when called.
 * @return 0 for succes and -1 for error
 */
int aeron_add_available_counter_handler(aeron_t *client, aeron_on_available_counter_pair_t *pair);

/**
 * Remove a previously added handler to be called when a new counter becomes available.
 *
 * NOTE: This function blocks until the handler is removed by the client conductor thread.
 *
 * @param client for the counter
 * @param pair holding the handler to call and a clientd to pass when called.
 * @return 0 for succes and -1 for error
 */
int aeron_remove_available_counter_handler(aeron_t *client, aeron_on_available_counter_pair_t *pair);

/**
 * Add a handler to be called when a new counter becomes unavailable or goes away.
 *
 * NOTE: This function blocks until the handler is added by the client conductor thread.
 *
 * @param client for the counter
 * @param pair holding the handler to call and a clientd to pass when called.
 * @return 0 for succes and -1 for error
 */
int aeron_add_unavailable_counter_handler(aeron_t *client, aeron_on_unavailable_counter_pair_t *pair);

/**
 * Remove a previously added handler to be called when a new counter becomes unavailable or goes away.
 *
 * NOTE: This function blocks until the handler is removed by the client conductor thread.
 *
 * @param client for the counter
 * @param pair holding the handler to call and a clientd to pass when called.
 * @return 0 for succes and -1 for error
 */
int aeron_remove_unavailable_counter_handler(aeron_t *client, aeron_on_unavailable_counter_pair_t *pair);

/**
 * Add a handler to be called when client is closed.
 *
 * NOTE: This function blocks until the handler is added by the client conductor thread.
 *
 * @param client for the counter
 * @param pair holding the handler to call and a clientd to pass when called.
 * @return 0 for succes and -1 for error
 */
int aeron_add_close_handler(aeron_t *client, aeron_on_close_client_pair_t *pair);

/**
 * Remove a previously added handler to be called when client is closed.
 *
 * NOTE: This function blocks until the handler is removed by the client conductor thread.
 *
 * @param client for the counter
 * @param pair holding the handler to call and a clientd to pass when called.
 * @return 0 for succes and -1 for error
 */
int aeron_remove_close_handler(aeron_t *client, aeron_on_close_client_pair_t *pair);

/*
 * Counters Reader functions
 */

/**
 * Function called by aeron_counters_reader_foreach_counter for each counter in the aeron_counters_reader_t.
 *
 * @param value of the counter.
 * @param id of the counter.
 * @param label for the counter.
 * @param label_length for the counter.
 * @param clientd to be returned in the call
 */
typedef void (*aeron_counters_reader_foreach_counter_func_t)(
    int64_t value, int32_t id, const char *label, size_t label_length, void *clientd);

/**
 * Iterate over the coounters in the counters_reader and call the given function for each counter.
 *
 * @param counters_reader to iterate over.
 * @param func to call for each counter.
 * @param clientd to pass for each call to func.
 */
void aeron_counters_reader_foreach_counter(
    aeron_counters_reader_t *counters_reader,
    aeron_counters_reader_foreach_counter_func_t func,
    void *clientd);

/*
 * TLM 2020-05-25: More functions to be added for counters reader. Feel free to send us feedback.
 */

/*
 * Publication functions
 */

/**
 * The publication is not connected to a subscriber, this can be an intermittent state as subscribers come and go.
 */
#define AERON_PUBLICATION_NOT_CONNECTED (-1L)

/**
 * The offer failed due to back pressure from the subscribers preventing further transmission.
 */
#define AERON_PUBLICATION_BACK_PRESSURED (-2L)

/**
 * The offer failed due to an administration action and should be retried.
 * The action is an operation such as log rotation which is likely to have succeeded by the next retry attempt.
 */
#define AERON_PUBLICATION_ADMIN_ACTION (-3L)

/**
 * The publication has been closed and should no longer be used.
 */
#define AERON_PUBLICATION_CLOSED (-4L)

/**
 * The offer failed due to reaching the maximum position of the stream given term buffer length times the total
 * possible number of terms.
 * <p>
 * If this happens then the publication should be closed and a new one added. To make it less likely to happen then
 * increase the term buffer length.
 */
#define AERON_PUBLICATION_MAX_POSITION_EXCEEDED (-5L)

/**
 * An error has occurred. Such as a bad argument.
 */
#define AERON_PUBLICATION_ERROR (-6L)

/**
 * Function called when filling in the reserved value field of a message.
 *
 * @param clientd passed to the offer function.
 * @param buffer of the entire frame, including Aeron data header.
 * @param frame_length of the enture frame.
 */
typedef int64_t (*aeron_reserved_value_supplier_t)(void *clientd, uint8_t *buffer, size_t frame_length);

/**
 * Structure to hold pointer to a buffer and the buffer length.
 *
 * TLM 2020-05-25: want to make this a typedef of iovec when already defined as an option.
 */
typedef struct aeron_iovec_stct
{
    uint8_t *iov_base;
    size_t iov_len;
}
aeron_iovec_t;

/**
 * Structure used to hold information for a try_claim function call.
 */
typedef struct aeron_buffer_claim_stct
{
    uint8_t *frame_header;
    uint8_t *data;
    size_t length;
}
aeron_buffer_claim_t;

/**
 * Commit the given buffer_claim as a complete message available for consumption.
 *
 * @param buffer_claim to commit.
 * @return 0 for success or -1 for error.
 */
int aeron_buffer_claim_commit(aeron_buffer_claim_t *buffer_claim);

/**
 * Abort the given buffer_claim and assign its position as padding.
 *
 * @param buffer_claim to abort.
 * @return 0 for success or -1 for error.
 */
int aeron_buffer_claim_abort(aeron_buffer_claim_t *buffer_claim);

/**
 * Non-blocking publish of a buffer containing a message.
 *
 * @param publication to publish on.
 * @param buffer to publish.
 * @param length of the buffer.
 * @param reserved_value_supplier to use for setting the reserved fvalue field or NULL.
 * @param clientd to pass to the reserved_value_supplier.
 * @return the new stream position otherwise a negative error value.
 */
int64_t aeron_publication_offer(
    aeron_publication_t *publication,
    const uint8_t *buffer,
    size_t length,
    aeron_reserved_value_supplier_t reserved_value_supplier,
    void *clientd);

/**
 * Non-blocking publish by gathering buffer vectors into a message.
 *
 * @param publication to publish on.
 * @param iov array for the vectors
 * @param iovcnt of the number of vectors
 * @param reserved_value_supplier to use for setting the reserved fvalue field or NULL.
 * @param clientd to pass to the reserved_value_supplier.
 * @return the new stream position otherwise a negative error value.
 */
int64_t aeron_publication_offerv(
    aeron_publication_t *publication,
    aeron_iovec_t *iov,
    size_t iovcnt,
    aeron_reserved_value_supplier_t reserved_value_supplier,
    void *clientd);

/*
 * Try to claim a range in the publication log into which a message can be written with zero copy semantics.
 * Once the message has been written then aeron_buffer_claim_commit should be called thus making it available.
 * A claim length cannot be greater than max payload length.
 * <p>
 * <b>Note:</b> This method can only be used for message lengths less than MTU length minus header.
 * If the claim is held for more than the aeron.publication.unblock.timeout system property then the driver will
 * assume the publication thread is dead and will unblock the claim thus allowing other threads to make progress
 * and other claims to be sent to reach end-of-stream (EOS).
 *
 * @code
 * aeron_buffer_claim_t buffer_claim;
 *
 * if (aeron_publication_try_claim(publication, length, &buffer_claim) > 0L)
 * {
 *     // work with buffer_claim->data directly.
 *     aeron_buffer_claim_commit(&buffer_claim);
 * }
 * @endcode
 *
 * @param publication to publish to.
 * @param length of the message.
 * @param buffer_claim to be populated if the claim succeeds.
 * @return the new stream position otherwise a negative error value.
 */
int64_t aeron_publication_try_claim(
    aeron_publication_t *publication,
    size_t length,
    aeron_buffer_claim_t *buffer_claim);

/*
 * Get the status of the media channel for this publication.
 * <p>
 * The status will be ERRORED (-1) if a socket exception occurs on setup and ACTIVE (1) if all is well.
 *
 * @param publication to check status of.
 * @return 1 for ACTIVE, -1 for ERRORED
 */
int64_t aeron_publication_channel_status(aeron_publication_t *publication);

/**
 * Has the publication seen an active Subscriber recently?
 *
 * @param publication to check.
 * @return true if this publication has recently seen an active subscriber otherwise false.
 */
bool aeron_publication_is_connected(aeron_publication_t *publication);

/**
 * Get the session id of the publication.
 *
 * @param publication to query
 * @return session id of the publication or -1 for error (Check aeron_errcode).
 */
int32_t aeron_publication_session_id(aeron_publication_t *publication);

/**
 * Add a destination manually to a multi-destination-cast publication.
 *
 * @param publication to add destination to.
 * @param uri for the destination to add.
 * @return 0 for success or -1 for error.
 */
int aeron_publication_add_destination(aeron_publication_t *publication, const char *uri);

/**
 * Remove a previously added destination manually from a multi-destination-cast publication.
 *
 * @param publication to remove destination from.
 * @param uri for the destination to remove.
 * @return 0 for success or -1 for error.
 */
int aeron_publication_remove_destination(aeron_publication_t *publication, const char *uri);

/**
 * Asynchronously close the publication.
 *
 * @param publication to close
 * @return 0 for success or -1 for error.
 */
int aeron_publication_close(aeron_publication_t *publication);

/*
 * TLM 2020-05-25: More functions to be added for publications. Feel free to send us feedback.
 */

/*
 * Exclusive Publication functions
 */

/**
 * Non-blocking publish of a buffer containing a message.
 *
 * @param publication to publish on.
 * @param buffer to publish.
 * @param length of the buffer.
 * @param reserved_value_supplier to use for setting the reserved fvalue field or NULL.
 * @param clientd to pass to the reserved_value_supplier.
 * @return the new stream position otherwise a negative error value.
 */
int64_t aeron_exclusive_publication_offer(
    aeron_exclusive_publication_t *publication,
    const uint8_t *buffer,
    size_t length,
    aeron_reserved_value_supplier_t reserved_value_supplier,
    void *clientd);

/**
 * Non-blocking publish by gathering buffer vectors into a message.
 *
 * @param publication to publish on.
 * @param iov array for the vectors
 * @param iovcnt of the number of vectors
 * @param reserved_value_supplier to use for setting the reserved fvalue field or NULL.
 * @param clientd to pass to the reserved_value_supplier.
 * @return the new stream position otherwise a negative error value.
 */
int64_t aeron_exclusive_publication_offerv(
    aeron_exclusive_publication_t *publication,
    aeron_iovec_t *iov,
    size_t iovcnt,
    aeron_reserved_value_supplier_t reserved_value_supplier,
    void *clientd);

/*
 * Try to claim a range in the publication log into which a message can be written with zero copy semantics.
 * Once the message has been written then aeron_buffer_claim_commit should be called thus making it available.
 * A claim length cannot be greater than max payload length.
 * <p>
 * <b>Note:</b> This method can only be used for message lengths less than MTU length minus header.
 *
 * @code
 * aeron_buffer_claim_t buffer_claim;
 *
 * if (aeron_exclusive_publication_try_claim(publication, length, &buffer_claim) > 0L)
 * {
 *     // work with buffer_claim->data directly.
 *     aeron_buffer_claim_commit(&buffer_claim);
 * }
 * @endcode
 *
 * @param publication to publish to.
 * @param length of the message.
 * @param buffer_claim to be populated if the claim succeeds.
 * @return the new stream position otherwise a negative error value.
 */
int64_t aeron_exclusive_publication_try_claim(
    aeron_exclusive_publication_t *publication,
    size_t length,
    aeron_buffer_claim_t *buffer_claim);

/**
 * Append a padding record log of a given length to make up the log to a position.
 *
 * @param length of the range to claim, in bytes.
 * @return the new stream position otherwise a negative error value.
 */
int64_t aeron_exclusive_publication_append_padding(
    aeron_exclusive_publication_t *publication,
    size_t length);

/**
 * Offer a block of pre-formatted message fragments directly into the current term.
 *
 * @param buffer containing the pre-formatted block of message fragments.
 * @param offset offset in the buffer at which the first fragment begins.
 * @param length in bytes of the encoded block.
 * @return the new stream position otherwise a negative error value.
 */
int64_t aeron_exclusive_publication_offer_block(
    aeron_exclusive_publication_t *publication,
    const uint8_t *buffer,
    size_t length);

/*
 * Get the status of the media channel for this publication.
 * <p>
 * The status will be ERRORED (-1) if a socket exception occurs on setup and ACTIVE (1) if all is well.
 *
 * @param publication to check status of.
 * @return 1 for ACTIVE, -1 for ERRORED
 */
int64_t aeron_exclusive_publication_channel_status(aeron_exclusive_publication_t *publication);

/**
 * Get the session id of the publication.
 *
 * @param publication to query
 * @return session id of the publication or -1 for error (Check aeron_errcode).
 */
int32_t aeron_exclusive_publication_session_id(aeron_exclusive_publication_t *publication);

/**
 * Add a destination manually to a multi-destination-cast publication.
 *
 * @param publication to add destination to.
 * @param uri for the destination to add.
 * @return 0 for success or -1 for error.
 */
int aeron_exclusive_publication_add_destination(aeron_exclusive_publication_t *exclusive, const char *uri);

/**
 * Remove a previously added destination manually from a multi-destination-cast publication.
 *
 * @param publication to remove destination from.
 * @param uri for the destination to remove.
 * @return 0 for success or -1 for error.
 */
int aeron_exclusive_publication_remove_destination(aeron_exclusive_publication_t *exclusive, const char *uri);

/**
 * Asynchronously close the publication.
 *
 * @param publication to close
 * @return 0 for success or -1 for error.
 */
int aeron_exclusive_publication_close(aeron_exclusive_publication_t *publication);

/*
 * TLM 2020-05-25: More functions to be added for exclusive publications. Feel free to send us feedback.
 */

/*
 * Subscription functions
 *
 * Aeron Subscriber API for receiving a reconstructed image for a stream of messages from publishers on
 * a given channel and stream id pair. Images are aggregated under a subscription.
 * <p>
 * Subscription are created via an aeron_t object, and received messages are delivered
 * to the fragment handler.
 * <p>
 * By default fragmented messages are not reassembled before delivery. If an application must
 * receive whole messages, whether or not they were fragmented, then the subscriber
 * should be created with a fragment assembler or a custom implementation.
 * <p>
 * It is an application's responsibility to poll the subscription for new messages.
 * <p>
 * <b>Note:</b>Subscriptions are not threadsafe and should not be shared between subscribers.
 */

/**
 * Callback for handling fragments of data being read from a log.
 *
 * The frame will either contain a whole message or a fragment of a message to be reassembled. Messages are fragmented
 * if greater than the frame for MTU in length.
 *
 * @param clientd passed to the poll function.
 * @param buffer containing the data.
 * @param length of the data in bytes.
 * @param header representing the meta data for the data.
 */
typedef void (*aeron_fragment_handler_t)(
    void *clientd, const uint8_t *buffer, size_t length, aeron_header_t *header);

typedef enum aeron_controlled_fragment_handler_action_en
{
    /**
     * Abort the current polling operation and do not advance the position for this fragment.
     */
    AERON_ACTION_ABORT,

    /**
     * Break from the current polling operation and commit the position as of the end of the current fragment
     * being handled.
     */
    AERON_ACTION_BREAK,

    /**
     * Continue processing but commit the position as of the end of the current fragment so that
     * flow control is applied to this point.
     */
    AERON_ACTION_COMMIT,

    /**
     * Continue processing until fragment limit or no fragments with position commit at end of poll as in
     * aeron_fragment_handler_t.
     */
    AERON_ACTION_CONTINUE
}
aeron_controlled_fragment_handler_action_t;

/**
 * Callback for handling fragments of data being read from a log.
 *
 * Handler for reading data that is coming from a log buffer. The frame will either contain a whole message
 * or a fragment of a message to be reassembled. Messages are fragmented if greater than the frame for MTU in length.
 *
 * @param clientd passed to the controlled poll function.
 * @param buffer containing the data.
 * @param length of the data in bytes.
 * @param header representing the meta data for the data.
 * @return The action to be taken with regard to the stream position after the callback.
 */
typedef aeron_controlled_fragment_handler_action_t (*aeron_controlled_fragment_handler_t)(
    void *clientd, const uint8_t *buffer, size_t length, aeron_header_t *header);

/**
 * Callback for handling a block of messages being read from a log.
 *
 * @param clientd passed to the block poll function.
 * @param buffer containing the block of message fragments.
 * @param offset at which the block begins, including any frame headers.
 * @param length of the block in bytes, including any frame headers that is aligned.
 * @param session_id of the stream containing this block of message fragments.
 * @param term_id of the stream containing this block of message fragments.
 */
typedef void (*aeron_block_handler_t)(
    void *clientd, const uint8_t *buffer, size_t length, int32_t session_id, int32_t term_id);

/**
 * Get session id field from the message header.
 *
 * @param header to query.
 * @return session id field or -1 for error (Check aeron_errcode).
 */
int32_t aeron_header_session_id(aeron_header_t *header);

/**
 * Poll the images under the subscription for available message fragments.
 * <p>
 * Each fragment read will be a whole message if it is under MTU length. If larger than MTU then it will come
 * as a series of fragments ordered within a session.
 * <p>
 * To assemble messages that span multiple fragments then use aeron_fragment_asssembler_t.
 *
 * @param subscription to poll.
 * @param handler for handling each message fragment as it is read.
 * @param fragment_limit number of message fragments to limit when polling across multiple images.
 * @return the number of fragments received or -1 for error.
 */
int aeron_subscription_poll(
    aeron_subscription_t *subscription, aeron_fragment_handler_t handler, void *clientd, size_t fragment_limit);

/**
 * Poll in a controlled manner the images under the subscription for available message fragments.
 * Control is applied to fragments in the stream. If more fragments can be read on another stream
 * they will even if BREAK or ABORT is returned from the fragment handler.
 * <p>
 * Each fragment read will be a whole message if it is under MTU length. If larger than MTU then it will come
 * as a series of fragments ordered within a session.
 * <p>
 * To assemble messages that span multiple fragments then use aeron_controlled_fragment_assembler_t.
 *
 * @param subscription to poll.
 * @param handler for handling each message fragment as it is read.
 * @param fragment_limit number of message fragments to limit when polling across multiple images.
 * @return the number of fragments received or -1 for error.
 */
int aeron_subscription_controlled_poll(
    aeron_subscription_t *subscription, aeron_controlled_fragment_handler_t handler, void *clientd, size_t fragment_limit);

/**
 * Poll the images under the subscription for available message fragments in blocks.
 * <p>
 * This method is useful for operations like bulk archiving and messaging indexing.
 *
 * @param subscription to poll.
 * @param handler to receive a block of fragments from each image.
 * @param block_length_limit for each image polled.
 * @return the number of bytes consumed or -1 for error.
 */
long aeron_subscription_block_poll(
    aeron_subscription_t *subscription, aeron_block_handler_t handler, void *clientd, size_t block_length_limit);

/**
 * Is this subscription connected by having at least one open publication image.
 *
 * @param subscription to check.
 * @return true if this subscription connected by having at least one open publication image.
 */
bool aeron_subscription_is_connected(aeron_subscription_t *subscription);

/**
 * Count of images associated to this subscription.
 *
 * @param subscription to count images for.
 * @return count of count associated to this subscription or -1 for error.
 */
int aeron_subscription_image_count(aeron_subscription_t *subscription);

/**
 * Return the image associated with the given session_id under the given subscription.
 *
 * Note: the returned image is considered retained by the application and thus must be released via
 * aeron_image_release when finished or if the image becomes unavailable.
 *
 * @param subscription to search.
 * @param session_id associated with the image.
 * @return image associated with the given session_id or NULL if no image exists.
 */
aeron_image_t *aeron_subscription_image_by_session_id(aeron_subscription_t *subscription, int32_t session_id);

/**
 * Return the image at the given index.
 *
 * Note: the returned image is considered retained by the application and thus must be released via
 * aeron_image_release when finished or if the image becomes unavailable.
 *
 * @param subscription to search.
 * @param index for the image.
 * @return image at the given index or NULL if no image exists.
 */
aeron_image_t *aeron_subscription_image_at_index(aeron_subscription_t *subscription, size_t index);

/**
 * Iterate over the images for this subscription calling the given function.
 *
 * @param subscription to iterate over.
 * @param handler to be called for each image.
 */
void aeron_subscription_for_each_image(
    aeron_subscription_t *subscription, void (*handler)(aeron_image_t *image));

/**
 * Retain the given image for access in the application.
 *
 * Note: A retain call must have a corresponding release call.
 * Note: Subscriptions are not threadsafe and should not be shared between subscribers.
 *
 * @param subscription that image is part of.
 * @param image to retain
 * @return 0 for success and -1 for error.
 */
int aeron_subscription_image_retain(aeron_subscription_t *subscription, aeron_image_t *image);

/**
 * Release the given image and relinquish desire to use the image directly.
 *
 * Note: Subscriptions are not threadsafe and should not be shared between subscribers.
 *
 * @param subscription that image is part of.
 * @param image to release
 * @return 0 for succes and -1 for error.
 */
int aeron_subscription_image_release(aeron_subscription_t *subscription, aeron_image_t *image);

bool aeron_subscription_is_closed(aeron_subscription_t *subscription);

/*
 * Get the status of the media channel for this subscription.
 * <p>
 * The status will be ERRORED (-1) if a socket exception occurs on setup and ACTIVE (1) if all is well.
 *
 * @param subscription to check status of.
 * @return 1 for ACTIVE, -1 for ERRORED
 */
int64_t aeron_subscription_channel_status(aeron_subscription_t *subscription);

/**
 * Media address for delivery to the channel.
 *
 * @param subscription to query.
 * @return Media address for delivery to the channel or NULL for error.
 */
const char *aeron_subscription_channel(aeron_subscription_t *subscription);

/**
 * Stream identity for scoping within the channel media address.
 *
 * @param subscription to query.
 * @return Stream identity for scoping within the channel media address or -1 for error. Check aeron_errcode.
 */
int32_t aeron_subscription_stream_id(aeron_subscription_t *subscription);

/**
 * Add a destination manually to a multi-destination subscription.
 *
 * @param subscription to add destination to.
 * @param uri for the destination to add.
 * @return 0 for success and -1 for error.
 */
int aeron_subscription_add_destination(aeron_subscription_t *subscription, const char *uri);

/**
 * Remove a previously added destination from a multi-destination subscription.
 *
 * @param subscription to remove destination from.
 * @param uri for the destination to remove.
 * @return 0 for success and -1 for error.
 */
int aeron_subscription_remove_destination(aeron_subscription_t *subscription, const char *uri);

/**
 * Asynchronously close the subscription.
 *
 * @param subscription to close
 * @return 0 for success or -1 for error.
 */
int aeron_subscription_close(aeron_subscription_t *subscription);

/*
 * TLM 2020-05-25: More functions to be added for subscriptions. Feel free to send us feedback.
 */

/**
 * Image Functions
 *
 * Represents a replicated publication image from a publisher to a subscription.
 * Each image identifies a source publisher by session id.
 * <p>
 * By default fragmented messages are not reassembled before delivery. If an application must
 * receive whole messages, whether or not they were fragmented, then the subscriber
 * should be created with a fragment assembler or a custom implementation.
 * <p>
 * It is an application's responsibility to poll the image for new messages.
 * <p>
 * <b>Note:</b>Images are not threadsafe and should not be shared between subscribers.
 */

/**
 * The sessionId for the steam of messages. Sessions are unique within a subscription and unique across
 * all publications from a source identity.
 *
 * @param image to query.
 * @return the sessionId for the steam of messages or -1 for error (Check aeron_errcode).
 */
int32_t aeron_image_session_id(aeron_image_t *image);

/**
 * The source identity of the sending publisher as an abstract concept appropriate for the media.
 *
 * @param image to query.
 * @return source identity of the sending publisher as an abstract concept appropriate for the media or NULL for error.
 */
const char *aeron_image_source_identity(aeron_image_t *image);

/**
 * The position this image has been consumed to by the subscriber.
 *
 * @param image to query position of.
 * @return the position this image has been consumed to by the subscriber.
 */
int64_t aeron_image_position(aeron_image_t *image);

/**
 * Set the subscriber position for this image to indicate where it has been consumed to.
 *
 * @param image to set the position of.
 * @param new_position for the consumption point.
 */
int aeron_image_set_position(aeron_image_t *image, int64_t position);

/**
 * Is the current consumed position at the end of the stream?
 *
 * @param image to check.
 * @return true if at the end of the stream or false if not.
 */
bool aeron_image_is_end_of_stream(aeron_image_t *image);

/**
 * Count of observed active transports within the image liveness timeout.
 *
 * If the image is closed, then this is 0. This may also be 0 if no actual datagrams have arrived. IPC
 * Images also will be 0.
 *
 * @param image to check.
 * @return count of active transports - 0 if Image is closed, no datagrams yet, or IPC. Or -1 for error.
 */
int aeron_image_active_transport_count(aeron_image_t *image);

/**
 * Poll for new messages in a stream. If new messages are found beyond the last consumed position then they
 * will be delivered to the handler up to a limited number of fragments as specified.
 * <p>
 * Use a fragment assembler to assemble messages which span multiple fragments.
 *
 * @param image to poll.
 * @param handler to which message fragments are delivered.
 * @param clientd to pass to the handler.
 * @param fragment_limit for the number of fragments to be consumed during one polling operation.
 * @return the number of fragments that have been consumed or -1 for error.
 */
int aeron_image_poll(aeron_image_t *image, aeron_fragment_handler_t handler, void *clientd, size_t fragment_limit);

/**
 * Poll for new messages in a stream. If new messages are found beyond the last consumed position then they
 * will be delivered to the handler up to a limited number of fragments as specified.
 * <p>
 * Use a controlled fragment assembler to assemble messages which span multiple fragments.
 *
 * @param image to poll.
 * @param handler to which message fragments are delivered.
 * @param clientd to pass to the handler.
 * @param fragment_limit for the number of fragments to be consumed during one polling operation.
 * @return the number of fragments that have been consumed or -1 for error.
 */
int aeron_image_controlled_poll(
    aeron_image_t *image, aeron_controlled_fragment_handler_t handler, void *clientd, size_t fragment_limit);

/**
 * Poll for new messages in a stream. If new messages are found beyond the last consumed position then they
 * will be delivered to the handler up to a limited number of fragments as specified or the maximum position specified.
 * <p>
 * Use a fragment assembler to assemble messages which span multiple fragments.
 *
 * @param image to poll.
 * @param handler to which message fragments are delivered.
 * @param clientd to pass to the handler.
 * @param limit_position to consume messages up to.
 * @param fragment_limit for the number of fragments to be consumed during one polling operation.
 * @return the number of fragments that have been consumed or -1 for error.
 */
int aeron_image_bounded_poll(
    aeron_image_t *image,
    aeron_fragment_handler_t handler,
    void *clientd,
    int64_t limit_position,
    size_t fragment_limit);

/**
 * Poll for new messages in a stream. If new messages are found beyond the last consumed position then they
 * will be delivered to the handler up to a limited number of fragments as specified or the maximum position specified.
 * <p>
 * Use a controlled fragment assembler to assemble messages which span multiple fragments.
 *
 * @param image to poll.
 * @param handler to which message fragments are delivered.
 * @param clientd to pass to the handler.
 * @param limit_position to consume messages up to.
 * @param fragment_limit for the number of fragments to be consumed during one polling operation.
 * @return the number of fragments that have been consumed or -1 for error.
 */
int aeron_image_bounded_controlled_poll(
    aeron_image_t *image,
    aeron_controlled_fragment_handler_t handler,
    void *clientd,
    int64_t limit_position,
    size_t fragment_limit);

/**
 * Peek for new messages in a stream by scanning forward from an initial position. If new messages are found then
 * they will be delivered to the handler up to a limited position.
 * <p>
 * Use a controlled fragment assembler to assemble messages which span multiple fragments. Scans must also
 * start at the beginning of a message so that the assembler is reset.
 *
 * @param image to peek.
 * @param initial_position from which to peek forward.
 * @param handler to which message fragments are delivered.
 * @param clientd to pass to the handler.
 * @param limit_position up to which can be scanned.
 * @return the resulting position after the scan terminates which is a complete message or -1 for error.
 */
int64_t aeron_image_controlled_peek(
    aeron_image_t *image,
    int64_t initial_position,
    aeron_controlled_fragment_handler_t handler,
    void *clientd,
    int64_t limit_position);

/**
 * Poll for new messages in a stream. If new messages are found beyond the last consumed position then they
 * will be delivered to the handler up to a limited number of bytes.
 * <p>
 * A scan will terminate if a padding frame is encountered. If first frame in a scan is padding then a block
 * for the padding is notified. If the padding comes after the first frame in a scan then the scan terminates
 * at the offset the padding frame begins. Padding frames are delivered singularly in a block.
 * <p>
 * Padding frames may be for a greater range than the limit offset but only the header needs to be valid so
 * relevant length of the frame is data header length.
 *
 * @param image to poll.
 * @param handler to which block is delivered.
 * @param clientd to pass to the handler.
 * @param block_length_limit up to which a block may be in length.
 * @return the number of bytes that have been consumed or -1 for error.
 */
int aeron_image_block_poll(
    aeron_image_t *image, aeron_block_handler_t handler, void *clientd, size_t block_length_limit);

/*
 * TLM 2020-05-25: More functions to be added for images. Feel free to send us feedback.
 */

bool aeron_image_is_closed(aeron_image_t *image);

/**
 * A fragment handler that sits in a chain-of-responsibility pattern that reassembles fragmented messages
 * so that the next handler in the chain only sees whole messages.
 * <p>
 * Unfragmented messages are delegated without copy. Fragmented messages are copied to a temporary
 * buffer for reassembly before delegation.
 * <p>
 * The aeron_header_t passed to the delegate on assembling a message will be that of the last fragment.
 * <p>
 * Session based buffers will be allocated and grown as necessary based on the length of messages to be assembled.
 */

/**
 * Create an image fragment assembler for use with a single image.
 *
 * @param assembler to be set when created successfully.
 * @param delegate to call on completed
 * @param delegate_clientd to pass to delegate handler.
 * @return 0 for success and -1 for error.
 */
int aeron_image_fragment_assembler_create(
    aeron_image_fragment_assembler_t **assembler,
    aeron_fragment_handler_t delegate,
    void *delegate_clientd);

/**
 * Delete an image fragment assembler.
 *
 * @param assembler to delete.
 * @return 0 for success or -1 for error.
 */
int aeron_image_fragment_assembler_delete(aeron_image_fragment_assembler_t *assembler);

/**
 * Handler function to be passed for handling fragment assembly.
 *
 * @param clientd passed in the poll call (must be a aeron_image_fragment_assembler_t)
 * @param buffer containing the data.
 * @param length of the data in bytes.
 * @param header representing the meta data for the data.
 */
void aeron_image_fragment_assembler_handler(
    void *clientd, const uint8_t *buffer, size_t length, aeron_header_t *header);

/**
 * Create an image controlled fragment assembler for use with a single image.
 *
 * @param assembler to be set when created successfully.
 * @param delegate to call on completed
 * @param delegate_clientd to pass to delegate handler.
 * @return 0 for success and -1 for error.
 */
int aeron_image_controlled_fragment_assembler_create(
    aeron_image_controlled_fragment_assembler_t **assembler,
    aeron_controlled_fragment_handler_t delegate,
    void *delegate_clientd);

/**
 * Delete an image controlled fragment assembler.
 *
 * @param assembler to delete.
 * @return 0 for success or -1 for error.
 */
int aeron_image_controlled_fragment_assembler_delete(aeron_image_controlled_fragment_assembler_t *assembler);

/**
 * Handler function to be passed for handling fragment assembly.
 *
 * @param clientd passed in the poll call (must be a aeron_image_controlled_fragment_assembler_t)
 * @param buffer containing the data.
 * @param length of the data in bytes.
 * @param header representing the meta data for the data.
 * @return The action to be taken with regard to the stream position after the callback.
 */
aeron_controlled_fragment_handler_action_t aeron_controlled_image_fragment_assembler_handler(
    void *clientd, const uint8_t *buffer, size_t length, aeron_header_t *header);

/**
 * Create a fragment assembler for use with a subscription.
 *
 * @param assembler to be set when created successfully.
 * @param delegate to call on completed
 * @param delegate_clientd to pass to delegate handler.
 * @return 0 for success and -1 for error.
 */
int aeron_fragment_assembler_create(
    aeron_fragment_assembler_t **assembler,
    aeron_fragment_handler_t delegate,
    void *delegate_clientd);

/**
 * Delete a fragment assembler.
 *
 * @param assembler to delete.
 * @return 0 for success or -1 for error.
 */
int aeron_fragment_assembler_delete(aeron_fragment_assembler_t *assembler);

/**
 * Handler function to be passed for handling fragment assembly.
 *
 * @param clientd passed in the poll call (must be a aeron_fragment_assembler_t)
 * @param buffer containing the data.
 * @param length of the data in bytes.
 * @param header representing the meta data for the data.
 */
void aeron_fragment_assembler_handler(
    void *clientd, const uint8_t *buffer, size_t length, aeron_header_t *header);

/**
 * Create a controlled fragment assembler for use with a subscription.
 *
 * @param assembler to be set when created successfully.
 * @param delegate to call on completed
 * @param delegate_clientd to pass to delegate handler.
 * @return 0 for success and -1 for error.
 */
int aeron_controlled_fragment_assembler_create(
    aeron_controlled_fragment_assembler_t **assembler,
    aeron_controlled_fragment_handler_t delegate,
    void *delegate_clientd);

/**
 * Delete a controlled fragment assembler.
 *
 * @param assembler to delete.
 * @return 0 for success or -1 for error.
 */
int aeron_controlled_fragment_assembler_delete(aeron_controlled_fragment_assembler_t *assembler);

/**
 * Handler function to be passed for handling fragment assembly.
 *
 * @param clientd passed in the poll call (must be a aeron_controlled_fragment_assembler_t)
 * @param buffer containing the data.
 * @param length of the data in bytes.
 * @param header representing the meta data for the data.
 * @return The action to be taken with regard to the stream position after the callback.
 */
aeron_controlled_fragment_handler_action_t aeron_controlled_fragment_assembler_handler(
    void *clientd, const uint8_t *buffer, size_t length, aeron_header_t *header);

/*
* Counter functions
*/

/**
 * Return a pointer to the counter value.
 *
 * @param counter to pointer to.
 * @return pointer to the counter value.
 */
int64_t *aeron_counter_addr(aeron_counter_t *counter);

/**
 * Asynchronously close the counter.
 *
 * @param counter to close
 * @return 0 for success or -1 for error.
 */
int aeron_counter_close(aeron_counter_t *counter);

/**
 * Return full version and build string.
 *
 * @return full version and build string.
 */
const char *aeron_version_full();

/**
 * Return major version number.
 *
 * @return major version number.
 */
int aeron_version_major();

/**
 * Return minor version number.
 *
 * @return minor version number.
 */
int aeron_version_minor();

/**
 * Return patch version number.
 *
 * @return patch version number.
 */
int aeron_version_patch();

/**
 * Clock function used by aeron.
 */
typedef int64_t (*aeron_clock_func_t)();

/**
 * Return time in nanoseconds for machine. Is not wall clock time.
 *
 * @return nanoseconds since epoch for machine.
 */
int64_t aeron_nano_clock();

/**
 * Return time in milliseconds since epoch. Is wall clock time.
 *
 * @return milliseconds since epoch.
 */
int64_t aeron_epoch_clock();

/**
 * Function to return logging information.
 */
typedef void (*aeron_log_func_t)(const char *);

/**
 * Determine if an aeron driver is using a given aeron directory.
 *
 * @param dirname  for aeron directory
 * @param timeout_ms  to use to determine activity for aeron directory
 * @param log_func to call during activity check to log diagnostic information.
 * @return true for active driver or false for no active driver.
 */
bool aeron_is_driver_active(const char *dirname, int64_t timeout_ms, aeron_log_func_t log_func);

/**
 * Load properties from a string containing name=value pairs and set appropriate environment variables for the
 * process so that subsequent calls to aeron_driver_context_init will use those values.
 *
 * @param buffer containing properties and values.
 * @return 0 for success and -1 for error.
 */
int aeron_properties_buffer_load(const char *buffer);

/**
 * Load properties file and set appropriate environment variables for the process so that subsequent
 * calls to aeron_driver_context_init will use those values.
 *
 * @param filename to load.
 * @return 0 for success and -1 for error.
 */
int aeron_properties_file_load(const char *filename);

/**
 * Load properties from HTTP URL and set environment variables for the process so that subsequent
 * calls to aeron_driver_context_init will use those values.
 *
 * @param url to attempt to retrieve and load.
 * @return 0 for success and -1 for error.
 */
int aeron_properties_http_load(const char *url);

/**
 * Load properties based on URL or filename. If string contains file or http URL, it will attempt
 * to load properties from a file or http as indicated. If not a URL, then it will try to load the string
 * as a filename.
 *
 * @param url_or_filename to load properties from.
 * @return 0 for success and -1 for error.
 */
int aeron_properties_load(const char *url_or_filename);

/**
 * Return current aeron error code (errno) for calling thread.
 *
 * @return aeron error code for calling thread.
 */
int aeron_errcode();

/**
 * Return the current aeron error message for calling thread.
 *
 * @return aeron error message for calling thread.
 */
const char *aeron_errmsg();

#ifdef __cplusplus
}
#endif

#endif //AERON_C_H
