/*
This example reads standard from input and writes
to the default PCM device for 15 seconds of data.

*/

/* Use the newer ALSA API */
#define ALSA_PCM_NEW_HW_PARAMS_API

#include <alsa/asoundlib.h>
#include <getopt.h>

//static char *device = "plughw:0,0";         /* playback device */
static char *device = "default";
static snd_pcm_format_t format = SND_PCM_FORMAT_S16;    /* sample format */
static unsigned int rate = 44100;           /* stream rate */
static unsigned int channels = 1;           /* count of channels */
static unsigned int buffer_time = 500000;       /* ring buffer length in us */
static unsigned int period_time = 100000;       /* period time in us */
static int verbose = 0;                 /* verbose flag */
static int resample = 1;                /* enable alsa-lib resampling */
static int period_event = 0;                /* produce poll event after each period */
 
static snd_pcm_sframes_t buffer_size;
static snd_pcm_sframes_t period_size;

//#define DEBUG

#ifdef DEBUG
static snd_output_t *output = NULL;
#endif

static int set_hwparams(snd_pcm_t *handle,
            snd_pcm_hw_params_t *params,
            snd_pcm_access_t access)
{
    unsigned int rrate;
    snd_pcm_uframes_t size;
    int err, dir;
 
    /* choose all parameters */
    err = snd_pcm_hw_params_any(handle, params);
    if (err < 0) {
        printf("Broken configuration for playback: no configurations available: %s\n", snd_strerror(err));
        return err;
    }
    /* set hardware resampling */
    err = snd_pcm_hw_params_set_rate_resample(handle, params, resample);
    if (err < 0) {
        printf("Resampling setup failed for playback: %s\n", snd_strerror(err));
        return err;
    }
    /* set the interleaved read/write format */
    err = snd_pcm_hw_params_set_access(handle, params, access);
    if (err < 0) {
        printf("Access type not available for playback: %s\n", snd_strerror(err));
        return err;
    }
    /* set the sample format */
    err = snd_pcm_hw_params_set_format(handle, params, format);
    if (err < 0) {
        printf("Sample format not available for playback: %s\n", snd_strerror(err));
        return err;
    }
    /* set the count of channels */
    err = snd_pcm_hw_params_set_channels(handle, params, channels);
    if (err < 0) {
        printf("Channels count (%u) not available for playbacks: %s\n", channels, snd_strerror(err));
        return err;
    }
    /* set the stream rate */
    rrate = rate;
    err = snd_pcm_hw_params_set_rate_near(handle, params, &rrate, 0);
    if (err < 0) {
        printf("Rate %uHz not available for playback: %s\n", rate, snd_strerror(err));
        return err;
    }
    if (rrate != rate) {
        printf("Rate doesn't match (requested %uHz, get %iHz)\n", rate, err);
        return -EINVAL;
    }
    /* set the buffer time */
    err = snd_pcm_hw_params_set_buffer_time_near(handle, params, &buffer_time, &dir);
    if (err < 0) {
        printf("Unable to set buffer time %u for playback: %s\n", buffer_time, snd_strerror(err));
        return err;
    }
    err = snd_pcm_hw_params_get_buffer_size(params, &size);
    if (err < 0) {
        printf("Unable to get buffer size for playback: %s\n", snd_strerror(err));
        return err;
    }
    buffer_size = size;
    /* set the period time */
    err = snd_pcm_hw_params_set_period_time_near(handle, params, &period_time, &dir);
    if (err < 0) {
        printf("Unable to set period time %u for playback: %s\n", period_time, snd_strerror(err));
        return err;
    }
    err = snd_pcm_hw_params_get_period_size(params, &size, &dir);
    if (err < 0) {
        printf("Unable to get period size for playback: %s\n", snd_strerror(err));
        return err;
    }
    period_size = size;
    /* write the parameters to device */
    err = snd_pcm_hw_params(handle, params);
    if (err < 0) {
        printf("Unable to set hw params for playback: %s\n", snd_strerror(err));
        return err;
    }
    return 0;
}

static int xrun_recovery(snd_pcm_t *handle, int err)
{
    if (verbose)
        printf("stream recovery\n");
    if (err == -EPIPE) {    /* under-run */
        err = snd_pcm_prepare(handle);
        if (err < 0)
            printf("Can't recovery from underrun, prepare failed: %s\n", snd_strerror(err));
        return 0;
    } else if (err == -ESTRPIPE) {
        while ((err = snd_pcm_resume(handle)) == -EAGAIN)
            sleep(1);   /* wait until the suspend flag is released */
        if (err < 0) {
            err = snd_pcm_prepare(handle);
            if (err < 0)
                printf("Can't recovery from suspend, prepare failed: %s\n", snd_strerror(err));
        }
        return 0;
    }
    return err;
}

struct async_private_data {
	char *buffer;
	int size;
	snd_pcm_uframes_t frames;
	long loops;
};

static void async_callback(snd_async_handler_t *ahandler)
{
    snd_pcm_t *handle = snd_async_handler_get_pcm(ahandler);
    struct async_private_data *data = snd_async_handler_get_callback_private(ahandler);
    char *buffer = data->buffer;
    snd_pcm_sframes_t avail;
    int err, size = data->size;
    
	//TODO 8: Check the available data with snd_pcm_avail_update and assign it to avail
    while (avail >= period_size) {
		if (data->loops <= 0) {
			//snd_async_del_handler(async_callback);
			break;
		}
		data->loops--; 
		err = read(0, buffer, size);
		if (err == 0) {
			fprintf(stderr, "end of file on input\n");
			break;
		} else if (err != size) {
			fprintf(stderr,
					"short read: read %d bytes\n", err);
			data->loops = 0;
		}
		//TODO 9: Write with period_size data with snd_pcm_writei
#ifdef DEBUG
		snd_pcm_dump(playback_handle, output);
#endif
        if (err < 0) {
            printf("Write error: %s\n", snd_strerror(err));
            exit(EXIT_FAILURE);
        }
        if (err != period_size) {
            printf("Write error: written %i expected %li\n", err, period_size);
            exit(EXIT_FAILURE);
        }
		//TODO 10: check the available data with snd_pcm_avail_update
    }
}

static int async_loop(snd_pcm_t *handle, char *buffer, int size,
		snd_pcm_uframes_t frames, long loops)
{
    struct async_private_data data;
    snd_async_handler_t *ahandler;
    int err, count;
	//TODO 6: Populate data.buffer, data.loops, data.frames and data.sizes
	
	//TODO 7: Setup the Async Handler with snd_async_add_pcm_handler 
    if (err < 0) {
        printf("Unable to register async handler\n");
        return -1;
    }
    for (count = 0; count < 2; count++) {
		err = read(0, buffer, size);
        err = snd_pcm_writei(handle, buffer, frames);
        if (err < 0) {
            printf("Initial write error: %s\n", snd_strerror(err));
            return -1;
        }
        if (err != frames) {
            printf("Initial write error: written %i expected %li\n", err, period_size);
            return -1;
        }
    }
    if (snd_pcm_state(handle) == SND_PCM_STATE_PREPARED) {
        err = snd_pcm_start(handle);
        if (err < 0) {
            printf("Start error: %s\n", snd_strerror(err));
            return -1;
        }
    }
 
    /* because all other work is done in the signal handler,
       suspend the process */
    while (1) {
        sleep(1);
		if (data.loops <= 0) {
			//snd_async_del_handler(async_callback);
			break;
		}
    }
	return 0;
}

static void help(void)
{
	int k;
	printf(
			"Usage: pcm [OPTION]... [FILE]...\n"
			"-h,--help  help\n"
			"-D,--device    playback device\n"
			"-r,--rate  stream rate in Hz\n"
			"-c,--channels  count of channels in stream\n"
			"-f,--frequency sine wave frequency in Hz\n"
			"-b,--buffer    ring buffer size in us\n"
			"-p,--period    period size in us\n"
			"-o,--format    sample format\n"
			"-v,--verbose   show the PCM setup parameters\n"
			"-e,--pevent    enable poll event after each period\n"
			"\n");
	printf("Recognized sample formats are:");
	for (k = 0; k < SND_PCM_FORMAT_LAST; ++k) {
		const char *s = snd_pcm_format_name(k);
		if (s)
			printf(" %s", s);
	}
	printf("\n");
}

int main(int argc, char *argv[])
{
	struct option long_option[] =
	{
		{"help", 0, NULL, 'h'},
		{"device", 1, NULL, 'D'},
		{"rate", 1, NULL, 'r'},
		{"channels", 1, NULL, 'c'},
		{"buffer", 1, NULL, 'b'},
		{"period", 1, NULL, 'p'},
		{"format", 1, NULL, 'o'},
		{"verbose", 1, NULL, 'v'},
		{"pevent", 1, NULL, 'e'},
		{NULL, 0, NULL, 0},
	};

	long loops;
	int size, bps;
	snd_pcm_t *handle;
	snd_pcm_hw_params_t *hwparams;
	unsigned int rrate;
	unsigned int val;
	snd_pcm_uframes_t frames;
	char *buffer;
	int err, dir, rc, morehelp = 0;

	while (1) {
		int c;
		if ((c = getopt_long(argc, argv, "hD:r:c:b:p:o:ve", long_option, NULL)) < 0)
			break;
		switch (c) {
			case 'h':
				morehelp++;
				break;
			case 'D':
				device = strdup(optarg);
				break;
			case 'r':
				rate = atoi(optarg);
				rate = rate < 4000 ? 4000 : rate;
				rate = rate > 196000 ? 196000 : rate;
				break;
			case 'c':
				channels = atoi(optarg);
				channels = channels < 1 ? 1 : channels;
				channels = channels > 1024 ? 1024 : channels;
				break;
			case 'b':
				buffer_time = atoi(optarg);
				buffer_time = buffer_time < 1000 ? 1000 : buffer_time;
				buffer_time = buffer_time > 1000000 ? 1000000 : buffer_time;
				break;
			case 'p':
				period_time = atoi(optarg);
				period_time = period_time < 1000 ? 1000 : period_time;
				period_time = period_time > 1000000 ? 1000000 : period_time;
				break;
			case 'o':
				for (format = 0; format < SND_PCM_FORMAT_LAST; format++) {
					const char *format_name = snd_pcm_format_name(format);
					if (format_name)
						if (!strcasecmp(format_name, optarg))
							break;
				}
				if (format == SND_PCM_FORMAT_LAST)
					format = SND_PCM_FORMAT_S16;
				if (!snd_pcm_format_linear(format) &&
						!(format == SND_PCM_FORMAT_FLOAT_LE ||
							format == SND_PCM_FORMAT_FLOAT_BE)) {
					printf("Invalid (non-linear/float) format %s\n",
							optarg);
					return 1;
				}
				break;
			case 'v':
				verbose = 1;
				break;
			case 'e':
				period_event = 1;
				break;
		}
	}

	if (morehelp) {
		help();
		return 0;
	}

	/* Open PCM device for playback. */
	err = snd_pcm_open(&handle, device, SND_PCM_STREAM_PLAYBACK, 0);
	if (err < 0) {
		fprintf(stderr, "unable to open pcm device: %s\n",
				snd_strerror(err));
		exit(1);
	}

	/* Allocate a hardware parameters object. */
	snd_pcm_hw_params_alloca(&hwparams);

	if ((err = set_hwparams(handle, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) 		
	{
		printf("Setting of hwparams failed: %s\n", snd_strerror(err));
		exit(EXIT_FAILURE);
	}

	/* Use a buffer large enough to hold one period */
	//TODO 1: Get the period size with snd_pcm_hw_params_get_period_size
	//TODO 2: Get the bytes per sample using snd_pcm_format_width and assign it to bps
	//TODO 3: Calculate the period size in bytes and assign it to size

	buffer = (char *) malloc(size);

	/* We want to loop for 5 seconds */
	//TODO 4: Get the period time using snd_pcm_hw_params_get_period_time

	/* 5 seconds in microseconds divided by
	 * period time */
	//TODO 5: Calculate the loops required for 5 seconds

#ifdef DEBUG
	err = snd_output_stdio_attach(&output, stdout, 0);
	if (err < 0) {
		printf("Output failed: %s\n", snd_strerror(err));
		return 0;
	}
	snd_pcm_dump(handle, output);
#endif
	async_loop(handle, buffer, size, frames, loops);
	snd_pcm_drain(handle);
	snd_pcm_close(handle);
	free(buffer);
	return 0;
}
