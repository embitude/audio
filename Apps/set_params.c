/*

This example opens the default PCM device, sets
some parameters, and then displays the value
of most of the hardware parameters. It does not
perform any sound playback or recording.

*/

/* Use the newer ALSA API */
#define ALSA_PCM_NEW_HW_PARAMS_API

/* All of the ALSA library API is defined
 * in this header */
#include <alsa/asoundlib.h>

int main(int argc, char *argv[]) 
{
	int rc;
	snd_pcm_t *handle;
	snd_pcm_hw_params_t *params;
	unsigned int val, val2;
	int dir;
	snd_pcm_uframes_t frames;

	if (argc < 2) {
		printf("Usage: %s <device>\n", argv[0]);
		return -1;
	}

	//TODO 1.1: Open PCM device with snd_pcm_open.
	// Set direction as SND_PCM_STREAM_PLAYBACK, & mode as 0
	if (rc < 0) {
		fprintf(stderr, "unable to open pcm device: %s\n", 
			snd_strerror(rc));
		exit(1);
	}

	/*
	 * TODO 1.2: Allocate the hardware params with 
	 * snd_pcm_hw_params_alloca
	*/

	/* 
	 * TODO 1.3: Fill it in with default values by using snd_pcm_hw_params_any
	 */

	/* Set the desired hardware parameters. */

	/* TODO 1.4: Set the Interleaved mode using snd_pcm_hw_params_set_access */

	/* 
     * TODO 1.5: Set the format as Signed 16-bit little-endian format
	 * Use snd_pcm_hw_params_set_format
	 */

	/* TODO 1.6: Set Two channels (stereo) with snd_pcm_hw_params_set_channels */

	/* 
     * TODO 1.7: 44100 bits/second sampling rate (CD quality)
	 * Use snd_pcm_hw_params_set_rate_near
     */

	/* 
	 * TODO 2.1: Try playing around with buffer time & Period Time
	 * Use snd_pcm_hw_params_set_buffer_time_near and
	 * snd_pcm_hw_params_set_period_time_near
	 */
	
#if 0
	val = 500000;
	rc = snd_pcm_hw_params_set_buffer_time_near(handle, params, &val, &dir);
	if (rc < 0) {
		fprintf(stderr, "unable to set hw parameters: %s\n",
			snd_strerror(rc));
		exit(1);
	}

	val = 100000;
	rc = snd_pcm_hw_params_set_period_time_near(handle, params, &val, &dir);
	if (rc < 0) {
		fprintf(stderr, "unable to set hw parameters: %s\n",
			snd_strerror(rc));
		exit(1);
	}
#endif

	/* TODO 1.8 Write the parameters to the driver with snd_pcm_hw_params */
	if (rc < 0) {
		fprintf(stderr, "unable to set hw parameters: %s\n",
			snd_strerror(rc));
		exit(1);
	}

	/* Display information about the PCM interface */

	printf("PCM handle name = '%s'\n",
			snd_pcm_name(handle));

	printf("PCM state = %s\n",
			snd_pcm_state_name(snd_pcm_state(handle)));

	snd_pcm_hw_params_get_access(params,
			(snd_pcm_access_t *) &val);
	printf("access type = %s\n",
			snd_pcm_access_name((snd_pcm_access_t)val));

	snd_pcm_hw_params_get_format(params, &val);
	printf("format = '%s' (%s)\n",
			snd_pcm_format_name((snd_pcm_format_t)val),
			snd_pcm_format_description(
				(snd_pcm_format_t)val));

	snd_pcm_hw_params_get_channels(params, &val);
	printf("channels = %d\n", val);

	snd_pcm_hw_params_get_rate(params, &val, &dir);
	printf("rate = %d bps\n", val);

	snd_pcm_hw_params_get_period_time(params,
			&val, &dir);
	printf("period time = %d us\n", val);

	snd_pcm_hw_params_get_period_size(params,
			&frames, &dir);
	printf("period size = %d frames\n", (int)frames);

	snd_pcm_hw_params_get_buffer_time(params,
			&val, &dir);
	printf("buffer time = %d us\n", val);

	snd_pcm_hw_params_get_buffer_size(params,
			(snd_pcm_uframes_t *) &val);
	printf("buffer size = %d frames\n", val);

	snd_pcm_hw_params_get_periods(params, &val, &dir);
	printf("periods per buffer = %d frames\n", val);

	snd_pcm_hw_params_get_rate_numden(params,
			&val, &val2);
	printf("exact rate = %d/%d bps\n", val, val2);

	return 0;
}
