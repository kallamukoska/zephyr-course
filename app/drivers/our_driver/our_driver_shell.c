#include <zephyr/shell/shell.h>
#include <zephyr/drivers/sensor.h>
#include <errno.h>

static int cmd_sensor_fetch(const struct shell* sh, size_t argc, char** argv)
{
    const char* name = argv[1];
    const struct device* dev = shell_device_get_binding(name);

    if(dev == NULL)
    {
        shell_error(sh, "Device %s not found", name);
        return -ENODEV;
    }

    int ret = sensor_sample_fetch(dev);
    if (ret < 0)
    {
        shell_error(sh, "Fetch failed: %d", ret);
        return ret;
    }
    shell_print(sh, "Fetch OK");

    return 0;
}

static int cmd_sensor_read(const struct shell* sh, size_t argc, char** argv)
{
    const char* name = argv[1];
    const struct device* dev = shell_device_get_binding(name);
    struct sensor_value sensorValue = {0};

    if(dev == NULL)
    {
        shell_error(sh, "Device %s not found", name);
        return -ENODEV;
    }

    int ret = sensor_channel_get(dev, SENSOR_CHAN_AMBIENT_TEMP, &sensorValue);
    if(ret < 0)
    {
        shell_error(sh, "Channel get failed: %d", ret);
        return ret;
    }
    shell_print(sh, "%d.%06d", sensorValue.val1, sensorValue.val2);

    return 0;
}

static int cmd_sensor_info(const struct shell* sh, size_t argc, char** argv)
{
    const char* name = argv[1];
    const struct device* dev = shell_device_get_binding(name);

    if(dev == NULL)
    {
        shell_error(sh, "Device %s not found", name);
        return -ENODEV;
    }

    shell_print(sh, "Name:  %s", dev->name);
    shell_print(sh, "Ready: %s", device_is_ready(dev) ? "yes" : "no");

    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE
(
    sub_sensor,
    SHELL_CMD_ARG(fetch, NULL,
                  "Fetch a sample from the sensor (calls sensor_sample_fetch).\n"
                  "Usage: sensor fetch <device>",
                  cmd_sensor_fetch, 2, 0),
    SHELL_CMD_ARG(read, NULL,
                  "Read the ambient temperature channel and print it (calls sensor_channel_get).\n"
                  "Usage: sensor read <device>",
                  cmd_sensor_read, 2, 0),
    SHELL_CMD_ARG(info, NULL,
                  "Print the device name and its ready state.\n"
                  "Usage: sensor info <device>",
                  cmd_sensor_info, 2, 0),
    SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(sensor, &sub_sensor,
                   "Sensor commands for our_driver.\n"
                   "Usage: sensor <fetch|read|info> <device>",
                   NULL);
