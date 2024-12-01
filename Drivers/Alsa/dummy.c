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
	spinlock_t lock;
	/* timer stuff */
	struct timer_list timer;
	unsigned int irq_pos;		/* fractional IRQ position */
	unsigned long last_jiffies;
	unsigned int rate;
	/* PCM parameters */
	unsigned int pcm_period_size;
	unsigned int pcm_bps;		/* bytes per second */
	unsigned int pcm_bpj;		/* bytes per jiffy */
	struct snd_pcm_substream *substream;
	unsigned int pcm_buffer_size;
	unsigned int buf_pos;	/* position in buffer */
	unsigned int running;
	unsigned int elapsed :1;
};

static void dummy_systimer_rearm(struct dummy_systimer_pcm *dpcm)
{
	unsigned long tick;
	
	/*
	 * TODO 4.15: Calculate the required ticks/jiffies to generate period_size bytes 
	 * For this, first subtract irq_pos from pcm_period_size and then divide it by pcm_bpj
	 * Finally, assign it to tick
	 */
	if (tick == 0) {
		tick = 2;
	};
	printk("## Tick = %lu\n", tick);
	//TODO 4.16: Start the timer. Use mod_timer
}

static void dummy_fill_capture_buf(struct dummy_systimer_pcm *dpcm, unsigned int bytes)
{
	char *dst = dpcm->substream->runtime->dma_area;
	int j = 0;

	if (dpcm->running) {
		for (j = 0; j < bytes; j++) {
			//TODO 5.2: Copy the counter to dst and increment the pointers accordingly
			dst[dpcm->buf_pos++] = j % 256;

			//TODO 5.3: Wrap the buf_pos if it has gone past the pcm_buffer_size
			if (dpcm->buf_pos >= dpcm->pcm_buffer_size)
				dpcm->buf_pos = 0;
		}
	}
}

static void dummy_systimer_update(struct dummy_systimer_pcm *dpcm)
{
	unsigned long delta;
	unsigned int last_pos;

	if (!dpcm->running)
		return;

	//TODO 4.19: Get the difference between current jiffies and last_jiffies. Assign it to delta
	if (!delta)
		return;

	//TODO 4.20: Add delta to last_jiffies 

	last_pos = dpcm->irq_pos;

	//TODO 4.21: Get the current irq_pos by adding delta worth of bytes to it. Use pcm_bpj to get bytes per jiffies

	//TODO 4.22: Get the difference between current irq_pos and last_post and assign it to delta

	//TODO 4.23: Update the buf_pos only if its playback. Wrap if it has crossed the buffer boundary
	
	//TODO 4.24: Check if the irq_pos is greater that pcm_period_size. If yes, than wrap it and set the dpcm->elapsed = 1

	//TODO 5.1 Fill the Capture buffer for the capture stream only
}

static int dummy_systimer_start(struct snd_pcm_substream *substream)
{
	//TODO 4.12: Get dummy_systimer_pcm from substream->runtime->private
	struct dummy_systimer_pcm *dpcm;
	spin_lock(&dpcm->lock);
	//TODO 4.13: Update dpcm->last_jiffies to current value of jiffies

	//TODO 4.14: Start the timer with dummy_system_rearm
	spin_unlock(&dpcm->lock);
	return 0;
}

static void dummy_systimer_callback(struct timer_list *t)
{
	//TODO 4.17: dummy_systimer_pcm using from_timer
	//from_timer(var, callback_timer, timer_fieldname)
	struct dummy_systimer_pcm *dpcm;
	unsigned long flags;
	int elapsed = 0;

	spin_lock_irqsave(&dpcm->lock, flags);
	//TODO 4.18: Invoke dummy_systimer_update to update the pointers and other stuff
	//TODO 4.25: Restart the timer by calling dummy_systimer_rearm 
	elapsed = dpcm->elapsed;
	dpcm->elapsed = 0;
	spin_unlock_irqrestore(&dpcm->lock, flags);
	//TODO 4.26: Signal period elapsed if period time has been elapsed & stream is running
	// Use api snd_pcm_period_elapsed to signal the period elapsed
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

static void dummy_systimer_free(struct snd_pcm_substream *substream)
{
	kfree(substream->runtime->private_data);
}

/*
 * PCM interface
 */
static int dummy_pcm_trigger(struct snd_pcm_substream *substream, int cmd)
{
	//TODO 4.11: Get the dummy_systimer_pcm data structure from runtime private data
	struct dummy_systimer_pcm *dpcm;

	printk("Driver: In %s\n", __func__);

	switch (cmd) {
	case SNDRV_PCM_TRIGGER_START:
	case SNDRV_PCM_TRIGGER_RESUME:
		printk("Trigger start received\n");
		//TODO 4.11: Start the timer, if 'running' field in dpcm is not set
		// and set the dpcm->running accordingly
		// Invoke dummy_systimer_start to start the timer and return the value returned by it
		return 0;
		break;
	case SNDRV_PCM_TRIGGER_STOP:
	case SNDRV_PCM_TRIGGER_SUSPEND:
		printk("Trigger stop received\n");
		//TODO 4.28: Delete the timer with del_timer with del_timer & update the 'running' field to 0
		return 0;
	}
	return -EINVAL;
}

static int dummy_pcm_prepare(struct snd_pcm_substream *substream)
{
	//TODO 4.5: Get the runtime & dummy_systimer_pcm from substream
	struct snd_pcm_runtime *runtime;
	struct dummy_systimer_pcm *dpcm;
	unsigned int bps; //bytes per Second
	unsigned int bpj; //bytes per jiffy
	printk("Driver: In %s\n", __func__);

	/*
     * TODO 4.6: Calculate bytes per seconds using runtime->rate, 
     * runtime->channels, runtime->format. For getting number of bits 
     * in sample, use snd_pcm_format_width. Assign the same to bps and 
	 * divide it by 8
     */
	//TODO 4.7: Calculate bytes per jiffy by diving bps by HZ and assign it to bpj

	printk("Preparing bps = %u, bpj = %u", bps, bpj);
	if (bps <= 0)
		return -EINVAL;

	//TODO 4.8: Initialize dpcm->buf_pos to 0

	//TODO 4.9: Initialize dpcm->pcm_buffer_size (buffer size in bytes)
	// Use frames_to_bytes to convert runtime buffer size (frames) to bytes (Refer include/sound/pcm.h)

	dpcm->irq_pos = 0;
	dpcm->elapsed = 0;

	dpcm->pcm_bps = bps;
	dpcm->pcm_bpj = bpj;
	//TODO 4.10: Initialize dpcm->pcm_period_size (period size in bytes)
	// Use frames_to_bytes to convert runtime period size (frames) to bytes

	return 0;
}

static snd_pcm_uframes_t dummy_pcm_pointer(struct snd_pcm_substream *substream)
{
	/*
	 * TODO 4.27: Get dummy_systimer_pcm from runtime private data
	 * and return buf_pos in frames, use bytes_to_frames for the same
	 */
	struct dummy_systimer_pcm *dpcm;
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
	return snd_pcm_lib_malloc_pages(substream, params_buffer_bytes(hw_params));
}

static int dummy_pcm_hw_free(struct snd_pcm_substream *substream)
{
	printk("Driver: In %s\n", __func__);
	//TODO 3.9: Free up the pages for buffer
	// Use snd_pcm_lib_free_pages and return the value returned by it
	return snd_pcm_lib_free_pages(substream);
}

static int dummy_pcm_open(struct snd_pcm_substream *substream)
{
	struct snd_dummy *dummy = snd_pcm_substream_chip(substream);
	struct snd_pcm_runtime *runtime = substream->runtime;
	int err = 0;

	printk("Driver: In %s\n", __func__);

	//TODO 3.6: Assign dummy->pcm_hw to runtime->hw
	runtime->hw = dummy->pcm_hw;

	//TODO: 4.1 Create the systimer by invoking dummy_systimer_create
	if (err < 0)
		return err;
	
	return 0;
}

static int dummy_pcm_close(struct snd_pcm_substream *substream)
{
	printk("Driver: In %s\n", __func__);
	//TODO 4.29: Free up the systimer data structure by calling dummy_systimer_free
	printk(KERN_ERR "Freeing\n");
	return 0;
}

//TODO 3.3: Populate the pcm operations
// Assign callback handlers for .open, .close, .hw_params, .hw_free, .prepare,
// .trigger, .pointer
static struct snd_pcm_ops dummy_pcm_ops = {
	.ioctl =	snd_pcm_lib_ioctl,
	.open = dummy_pcm_open,
	.close = dummy_pcm_close,
	.hw_params = dummy_pcm_hw_params,
	.hw_free = dummy_pcm_hw_free,
	.prepare = dummy_pcm_prepare,
	.trigger = dummy_pcm_trigger,
	.pointer = dummy_pcm_pointer,
};

static int snd_card_dummy_pcm(struct snd_dummy *dummy, int device,
			      int substreams)
{
	struct snd_pcm *pcm;
	int err = -1;

	//TODO 3.2: Create the new pcm device
	err = snd_pcm_new(dummy->card, "Dummy PCM", 0,
        substreams, substreams, &pcm);
	if (err < 0)
		return err;
	dummy->pcm = pcm;
	//TODO 3.4: Register the pcm operations for the pcm device
	// To be done separately for capture & playback
	snd_pcm_set_ops(pcm, SNDRV_PCM_STREAM_PLAYBACK, &dummy_pcm_ops);
	snd_pcm_set_ops(pcm, SNDRV_PCM_STREAM_CAPTURE, &dummy_pcm_ops);

	pcm->private_data = dummy;
	pcm->info_flags = 0;
	strcpy(pcm->name, "Dummy PCM");

	//TODO 3.7: Pre-allocate the space for buffers
	err = snd_pcm_lib_preallocate_pages_for_all(pcm, SNDRV_DMA_TYPE_CONTINUOUS, 
			snd_dma_continuous_data(GFP_KERNEL), MAX_BUFFER_SIZE, MAX_BUFFER_SIZE);
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
	err = snd_card_new(&devptr->dev, index[devptr->id], "dummy-card",
         THIS_MODULE, sizeof(dummy), &card);
	if (err < 0)
		return err;

	dummy = card->private_data;
	dummy->card = card;

	//TODO 2.2: Set the driver name, short name and long name for the card
	// card->driver, card->shortname and card->longname
	sprintf(card->driver, "%s", "dummy-driver");
	sprintf(card->shortname, "%s", "snd-dummy");
	sprintf(card->longname, "%s", "Test snd-dummy");

	// TODO 3.1: Create the PCM device and register the ops
	// This is done in snd_card_dummy_pcm. Let's invoke the same
	// Let's use device as 0 and number of substreams to 1
	err = snd_card_dummy_pcm(dummy, 0, 1);
	if (err < 0)
		goto __nodev;

	//TODO 3.5 : Set the PCM Hardware params. Assign the hw params to pcm_hw field
	dummy->pcm_hw = dummy_pcm_hardware;

	//TODO 2.3: Register the sound card 
	err = snd_card_register(card);
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
	snd_card_free(platform_get_drvdata(devptr));
	return 0;
}

#define SND_DUMMY_DRIVER	"snd_dummy"
// TODO 1.1: Populate the Platform Driver Data Structure
// Assign handlers to .probe and .remove and assign name to .driver.name
static struct platform_driver snd_dummy_driver = {
	.probe = snd_dummy_probe,
	.remove = snd_dummy_remove,
	.driver		= {
		.name = SND_DUMMY_DRIVER,
	},
};

static int __init alsa_card_dummy_init(void)
{
	int err = 0;

	// TODO 1.2: Register the platform driver
	err = platform_driver_register(&snd_dummy_driver);
	if (err < 0)
		return err;

	// TODO 1.3: Register the platform device and assign it devices
	// Use platform_device_register_simple
	devices = platform_device_register_simple(
                SND_DUMMY_DRIVER, 0,
                NULL, 0);

	if (IS_ERR(devices))
		return -ENODEV;

	return 0;
}

static void __exit alsa_card_dummy_exit(void)
{
	// TODO 1.4: Deregister the platform device
	platform_device_unregister(devices);

	// TODO 1.5: Deregister the platform driver
	platform_driver_unregister(&snd_dummy_driver);
}

module_init(alsa_card_dummy_init)
module_exit(alsa_card_dummy_exit)

MODULE_AUTHOR("Embitude Trainings <info@embitude.in>");
MODULE_DESCRIPTION("Dummy soundcard");
MODULE_LICENSE("GPL");

