#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>

//#define DEBUG

#ifdef DEBUG
static snd_output_t *output = NULL;
#endif

int main(int argc, char *argv[])
{
	int i;
	int err;
	short buf[128];
	snd_pcm_t *playback_handle;
	snd_pcm_hw_params_t *hw_params;
	unsigned int val = 44100;

	if (argc < 2) {
		printf("Usage: %s <device>\n", argv[0]);
		return -1;
	}

	//TODO 1.1: Open PCM device with snd_pcm_open.
	// Set direction as SND_PCM_STREAM_PLAYBACK, & mode as 0
	if (err  < 0) {
		fprintf (stderr, "cannot open audio device %s (%s)\n",  argv[1],
				snd_strerror (err));
		exit (1);
	}

	/*
	 * TODO 1.2: Allocate the hardware params with 
	 * snd_pcm_hw_params_alloca
	*/

	/* 
	 * TODO 1.3: Fill it in with default values by using snd_pcm_hw_params_any
	 */
	if (err < 0) {
		fprintf (stderr, "cannot initialize hardware parameter structure (%s)\n",
				snd_strerror (err));
		exit (1);
	}

	/* TODO 1.4: Set the Interleaved mode using snd_pcm_hw_params_set_access */
	if (err < 0) {
		fprintf (stderr, "cannot set access type (%s)\n",
				snd_strerror (err));
		exit (1);
	}

	/* 
     * TODO 1.5: Set the format as Signed 16-bit little-endian format
	 * Use snd_pcm_hw_params_set_format
	 */
	if (err < 0) {
		fprintf (stderr, "cannot set sample format (%s)\n",
				snd_strerror (err));
		exit (1);
	}

	/* 44100 bits/second sampling rate (CD quality) */
	/* 
     * TODO 1.6: 44100 bits/second sampling rate (CD quality)
	 * Use snd_pcm_hw_params_set_rate_near
     */
	if (err < 0) {
		fprintf (stderr, "cannot set sample rate (%s)\n",
				snd_strerror (err));
		exit (1);
	}

	/* Two channels (stereo) */
	/* TODO 1.7: Set Two channels (stereo) with snd_pcm_hw_params_set_channels */
	if (err < 0)
	{
		fprintf (stderr, "cannot set channel count (%s)\n",
				snd_strerror (err));
		exit (1);
	}
#if 0
    err = snd_pcm_hw_params_set_period_time_near(playback_handle, hw_params, &period_time, 0);
    if (err < 0) {
        printf("Unable to set period time %u for playback: %s\n", period_time, snd_strerror(err));
        return err;
    }
    err = snd_pcm_hw_params_set_buffer_time_near(playback_handle, hw_params, &buffer_time, 0);
    if (err < 0) {
        printf("Unable to set buffer time %u for playback: %s\n", buffer_time, snd_strerror(err));
        return err;
    }
#endif

	/* TODO 1.8 Write the parameters to the driver with snd_pcm_hw_params */
	if (err < 0) {
		fprintf (stderr, "cannot set parameters (%s)\n",
				snd_strerror (err));
		exit (1);
	}
	// TODO 1.9: Invoke snd_pcm_prepare
	if (err < 0) {
		fprintf (stderr, "cannot prepare audio interface for use (%s)\n",
				snd_strerror (err));
		exit (1);
	}

#ifdef DEBUG
	err = snd_output_stdio_attach(&output, stdout, 0);
	if (err < 0) {
		printf("Output failed: %s\n", snd_strerror(err));
		return 0;
	}
#endif

	for (i = 0; i < 10; ++i) {
		//TODO 1.10: Write 128 bytes using snd_pcm_writei 
		if ((err) != 128) {
			fprintf (stderr, "write to audio interface failed (%s)\n",
					snd_strerror (err));
			exit (1);
		}
#ifdef DEBUG
		snd_pcm_dump(playback_handle, output);
#endif
	}
	printf("Playback done\n");

	snd_pcm_close(playback_handle);
	exit (0);
}
