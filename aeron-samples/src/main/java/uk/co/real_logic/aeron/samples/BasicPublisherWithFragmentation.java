/*
 * Copyright 2014 Real Logic Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package uk.co.real_logic.aeron.samples;

import uk.co.real_logic.aeron.Aeron;
import uk.co.real_logic.aeron.Publication;
import uk.co.real_logic.agrona.CloseHelper;
import uk.co.real_logic.agrona.concurrent.UnsafeBuffer;
import uk.co.real_logic.aeron.driver.MediaDriver;
import uk.co.real_logic.aeron.tools.MessageStream;

import java.nio.ByteBuffer;
import java.util.concurrent.TimeUnit;

/**
 * Basic Aeron publisher application
 */
public class BasicPublisherWithFragmentation
{
    private static final int STREAM_ID = SampleConfiguration.STREAM_ID;
    private static final int STREAM_ID_2 = SampleConfiguration.STREAM_ID + 1;
    private static final String CHANNEL = SampleConfiguration.CHANNEL;
    private static final long NUMBER_OF_MESSAGES = SampleConfiguration.NUMBER_OF_MESSAGES;
    private static final long LINGER_TIMEOUT_MS = SampleConfiguration.LINGER_TIMEOUT_MS;

    private static final boolean EMBEDDED_MEDIA_DRIVER = SampleConfiguration.EMBEDDED_MEDIA_DRIVER;
    private static final UnsafeBuffer BUFFER = new UnsafeBuffer(ByteBuffer.allocateDirect(8192));
    private static final UnsafeBuffer BUFFER_2 = new UnsafeBuffer(ByteBuffer.allocateDirect(8192));

    public static void main(final String[] args) throws Exception
    {
        System.out.println("Publishing to " + CHANNEL + " on stream Id " + STREAM_ID);

        SamplesUtil.useSharedMemoryOnLinux();

        final MediaDriver driver = EMBEDDED_MEDIA_DRIVER ? MediaDriver.launch() : null;
        final Aeron.Context ctx = new Aeron.Context();

        try (final Aeron aeron = Aeron.connect(ctx);
             final Publication publication = aeron.addPublication(CHANNEL, STREAM_ID, 777);
        	 final Publication publication2 = aeron.addPublication(CHANNEL, STREAM_ID_2, 999))
        {
            for (int i = 0; i < 5; i++)
            {
            	MessageStream msgStream = new MessageStream(8192);
            	int len = msgStream.getNext(BUFFER);
            	int len2 = msgStream.getNext(BUFFER_2);
                //final String message = "Hello World! " + i;
                //BUFFER.putBytes(0, message.getBytes());

                //final String message2 = "Hello World from Second Publisher ! " + i;
                //BUFFER_2.putBytes(0, message2.getBytes());

                //System.out.print("offering " + i + "/" + NUMBER_OF_MESSAGES);
                boolean offerStatus = false;
                while (!offerStatus)
                {
	                final boolean result = publication.offer(BUFFER, 0, len);

	                if (!result)
	                {
	                    System.out.println(" ah? from first publisher!");
	                    offerStatus = false;
	                }
	                else
	                {
	                	offerStatus = true;
	                    System.out.println(" yay!");
	                }
	                final boolean result2 = publication2.offer(BUFFER_2, 0, len2);

	                if (!result2)
	                {
	                    System.out.println(" ah? from second publisher!");
	                    offerStatus = false;
	                }
	                else
	                {
	                	offerStatus = true;
	                	System.out.println(" yay!");
	                }
                }

                Thread.sleep(TimeUnit.SECONDS.toMillis(1));
            }

            System.out.println("Done sending.");

            if (0 < LINGER_TIMEOUT_MS)
            {
                System.out.println("Lingering for " + LINGER_TIMEOUT_MS + " milliseconds...");
                Thread.sleep(LINGER_TIMEOUT_MS);
            }
        }

        CloseHelper.quietClose(driver);
    }
}
