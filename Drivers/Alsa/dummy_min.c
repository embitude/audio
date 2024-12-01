#include <linux/init.h>
#include <linux/err.h>
#include <linux/platform_device.h>
#include <linux/jiffies.h>
#include <linux/slab.h>
#include <linux/time.h>
#include <linux/wait.h>
#include <linux/hrtimer.h>
#include <linux/math64.h>
#include <linux/module.h>
#include <sound/core.h>
#include <sound/control.h>
#include <sound/tlv.h>
#include <sound/pcm.h>
#include <sound/rawmidi.h>
#include <sound/info.h>
#include <sound/initval.h>

#define MAX_PCM_DEVICES		4
#define MAX_PCM_SUBSTREAMS	128
#define MAX_MIDI_DEVICES	2

/* defaults */
#define MAX_BUFFER_SIZE		(64*1024)
#define MIN_PERIOD_SIZE		64
#define MAX_PERIOD_SIZE		MAX_BUFFER_SIZE
#define USE_FORMATS 		(SNDRV_PCM_FMTBIT_U8 | SNDRV_PCM_FMTBIT_S16_LE)
#define USE_RATE		SNDRV_PCM_RATE_CONTINUOUS | SNDRV_PCM_RATE_8000_48000
#define USE_RATE_MIN		5500
#define USE_RATE_MAX		48000
#define USE_CHANNELS_MIN 	1
#define USE_CHANNELS_MAX 	2
#define USE_PERIODS_MIN 	1
#define USE_PERIODS_MAX 	1024

static int index[SNDRV_CARDS] = SNDRV_DEFAULT_IDX;	/* Index 0-MAX */

#define SND_DUMMY	"dummy"

static struct platform_device *devices;

struct snd_dummy {
	struct snd_card *card;
	struct snd_pcm *pcm;
	struct snd_pcm_hardware pcm_hw;
};

/*
 * system timer interface
 */
struct dummy_systimer_pcm {
	/* timer stuff */
	struct timer_list timer;
	/* PCM parameters */
	struct snd_pcm_substream *substream;
	unsigned int pcm_buffer_size;
	unsigned int buf_pos;	/* position in buffer */
	unsigned int running;
};
#if 0
static int dummy_systimer_prepare(struct snd_pcm_substream *substream)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct dummy_systimer_pcm *dpcm = runtime->private_data;
	dpcm->pcm_buffer_size = frames_to_bytes(runtime, runtime->buffer_size);
	dpcm->buf_pos = 0;
	return 0;
}
#endif

static void dummy_systimer_callback(struct timer_list *t)
{
	//TODO 4.9: dummy_systimer_pcm using from_timer
	//from_timer(var, callback_timer, timer_fieldname)
	struct dummy_systimer_pcm *dpcm;

	/*
	 * TODO 4.10: Advance the buf_pos by 960 bytes (48Khz,1ch, 16bit)
	 * Handle the buffer wrap-around, if any
	 */

	//TODO 4.11: Restart the timer for 10 msecs with mod_timer

	//TODO 4.12: Signal period elapsed with snd_pcm_period_elapsed
}

static int dummy_systimer_create(struct snd_pcm_substream *substream)
{
	struct dummy_systimer_pcm *dpcm;
	
	//TODO 4.2: Allocate memory for dpcm using kzalloc(size, GFP_KERNEL)

	if (!dpcm)
		return -ENOMEM;
	//TODO 4.3: Assign dpcm to substream->runtime->private_data
	// and substream to dpcm->substream
	//TODO 4.4: Register the timer with timer_setup
	return 0;
}

/*
 * PCM interface
 */
static int dummy_pcm_trigger(struct snd_pcm_substream *substream, int cmd)
{
	//TODO 4.6: Get the dummy_systimer_pcm data structure from runtime private data
	struct dummy_systimer_pcm *dpcm = substream->runtime->private_data;

	printk("Driver: In %s\n", __func__);

	switch (cmd) {
	case SNDRV_PCM_TRIGGER_START:
	case SNDRV_PCM_TRIGGER_RESUME:
		printk("Trigger start received\n");
		//TODO 4.8: Start the timer with mod_timer for 10 msecs
		// use msecs_to_jiffies. Do this only if dpcm->running is 0
		return 0;
		break;
	case SNDRV_PCM_TRIGGER_STOP:
	case SNDRV_PCM_TRIGGER_SUSPEND:
		printk("Trigger stop received\n");
		//TODO 4.14: Delete the timer with del_timer and set dpcm->running to 0
		return 0;
	}
	return -EINVAL;
}

static int dummy_pcm_prepare(struct snd_pcm_substream *substream)
{
	//TODO 4.5: Get the runtime & dummy_systimer_pcm from substream
	struct snd_pcm_runtime *runtime;
	struct dummy_systimer_pcm *dpcm;

	printk("Driver: In %s\n", __func__);
	//TODO 4.6: Get the buffer_size from runtime & 
	// Update dpcm->pcm_buffer_size with frame_to_bytes
	//TODO 4.7: Initialize the dpcm->buf_pos with 0
	return 0;
}

static snd_pcm_uframes_t dummy_pcm_pointer(struct snd_pcm_substream *substream)
{
	/*
	 * TODO 4.13: Get dummy_systimer_pcm from runtime private data
	 * and return buf_pos in frames, use bytes_to_frames for the same
	 */
	return 0;
}

static const struct snd_pcm_hardware dummy_pcm_hardware = {
	.info =			(SNDRV_PCM_INFO_MMAP |
				 SNDRV_PCM_INFO_INTERLEAVED |
				 SNDRV_PCM_INFO_RESUME |
				 SNDRV_PCM_INFO_MMAP_VALID),
	.formats =		USE_FORMATS,
	.rates =		USE_RATE,
	.rate_min =		USE_RATE_MIN,
	.rate_max =		USE_RATE_MAX,
	.channels_min =		USE_CHANNELS_MIN,
	.channels_max =		USE_CHANNELS_MAX,
	.buffer_bytes_max =	MAX_BUFFER_SIZE,
	.period_bytes_min =	MIN_PERIOD_SIZE,
	.period_bytes_max =	MAX_PERIOD_SIZE,
	.periods_min =		USE_PERIODS_MIN,
	.periods_max =		USE_PERIODS_MAX,
	.fifo_size =		0,
};

static int dummy_pcm_hw_params(struct snd_pcm_substream *substream,
			       struct snd_pcm_hw_params *hw_params)
{
	printk("Driver: In %s\n", __func__);
	//TODO 3.8: Allocate the pages for buffer
    // Use snd_pcm_lib_malloc_pages and return the value returned by it
	// For size, use params_buffer_bytes(hw_params)
	return 0;
}

static int dummy_pcm_hw_free(struct snd_pcm_substream *substream)
{
	printk("Driver: In %s\n", __func__);
	//TODO 3.9: Free up the pages for buffer
	// Use snd_pcm_lib_free_pages and return the value returned by it
	return 0;
}

static int dummy_pcm_open(struct snd_pcm_substream *substream)
{
	struct snd_dummy *dummy = snd_pcm_substream_chip(substream);
	struct snd_pcm_runtime *runtime = substream->runtime;
	int err = 0;

	printk("Driver: In %s\n", __func__);

	//TODO 3.6: Assign dummy->pcm_hw to runtime->hw

	//TODO: 4.1 Create the systimer by invoking dummy_systimer_create
	if (err < 0)
		return err;
	
	return 0;
}

static int dummy_pcm_close(struct snd_pcm_substream *substream)
{
	printk("Driver: In %s\n", __func__);
	//TODO 4.15: Free up the systimer datastucture
	printk(KERN_ERR "Freeing\n");
	return 0;
}

//TODO 3.3: Populate the pcm operations
// Assign callback handlers for .open, .close, .hw_params, .hw_free, .prepare,
// .trigger, .pointer
static struct snd_pcm_ops dummy_pcm_ops = {
	.ioctl =	snd_pcm_lib_ioctl,
};

static int snd_card_dummy_pcm(struct snd_dummy *dummy, int device,
			      int substreams)
{
	struct snd_pcm *pcm;
	int err = -1;

	//TODO 3.2: Create the new pcm device with snd_pcm_new
	if (err < 0)
		return err;
	dummy->pcm = pcm;
	//TODO 3.4: Register the pcm operations for the pcm device
	// To be done separately for capture (SNDRV_PCM_STREAM_PLAYBACK) & 
	// playback (SNDRV_PCM_STREAM_PLAYBACK)

	pcm->private_data = dummy;
	pcm->info_flags = 0;
	strcpy(pcm->name, "Dummy PCM");

	//TODO 3.7: Pre-allocate the space for buffers using 
	// snd_pcm_lib_preallocate_pages_for_all
	// Use SNDRV_DMA_TYPE_CONTINUOUS as a type
	// and snd_dma_continuous_data(GFP_KERNEL) for data
	if (err < 0)
		return err;

	return 0;
}

static int snd_dummy_probe(struct platform_device *devptr)
{
	struct snd_card *card;
	struct snd_dummy *dummy;
	int err = -1;

	printk("Driver: In %s\n", __func__);

	//TODO 2.1: Create the sound card instance
	// Alocate the space for struct snd_dummy as a part of this
	if (err < 0)
		return err;

	dummy = card->private_data;
	dummy->card = card;

	//TODO 2.2: Set the driver name, short name and long name for the card
	// card->driver, card->shortname and card->longname

	// TODO 3.1: Create the PCM device and register the ops
	// This is done in snd_card_dummy_pcm. Let's invoke the same
	// Let's use device as 0 and number of substreams to 1
	if (err < 0)
		goto __nodev;

	//TODO 3.5 : Set the PCM Hardware params by assigning 
	// the hw params to pcm_hw field of dummy

	//TODO 2.3: Register the sound card 
	if (err == 0) {
		platform_set_drvdata(devptr, card);
		return 0;
	}
      __nodev:
	snd_card_free(card);
	return err;
}

static int snd_dummy_remove(struct platform_device *devptr)
{
	printk("Driver: In %s\n", __func__);
	//TODO 2.4: Deregister the sound card 
	return 0;
}

#define SND_DUMMY_DRIVER	"snd_dummy"
// TODO 1.1: Populate the Platform Driver Data Structure
// Assign handlers to .probe and .remove and assign name to .driver.name
static struct platform_driver snd_dummy_driver = {
};

static int __init alsa_card_dummy_init(void)
{
	int err = 0;

	// TODO 1.2: Register the platform driver
	if (err < 0)
		return err;

	// TODO 1.3: Register the platform device and assign it devices
	// Use platform_device_register_simple
	if (IS_ERR(devices))
		return -ENODEV;

	return 0;
}

static void __exit alsa_card_dummy_exit(void)
{
	// TODO 1.4: Deregister the platform device

	// TODO 1.5: Deregister the platform driver
}

module_init(alsa_card_dummy_init)
module_exit(alsa_card_dummy_exit)

MODULE_AUTHOR("Embitude Trainings <info@embitude.in>");
MODULE_DESCRIPTION("Dummy soundcard");
MODULE_LICENSE("GPL");

