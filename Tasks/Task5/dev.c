#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/atomic.h>

#define MY_MAJOR 42
#define MY_MINOR 0
#define NUM_MINORS 2 /* Allow up to two different devices with major MY_MAJOR to exist in the system at a time. */
#define MODULE_NAME "MyDevice"
#define MESSAGE "hello\n"

#define BUFFER_SIZE 4096

MODULE_DESCRIPTION("Character Device Driver");
MODULE_AUTHOR("Me");
MODULE_LICENSE("GPL");


struct my_device_data {
    struct cdev cdev;  /* TODO Add the cdev member variable. */

    char buffer[BUFFER_SIZE];  /* TODO Add a buffer with BUFFER_SIZE elements member variable. */

    size_t size;  /* "size" variable holds the size of the buffer. */
    atomic_t access;  /* "access" variable is used so that this device can only be accessed by a single process at a time. */
};


struct my_device_data devs[NUM_MINORS];

/* This function will get called when an application tries to open the device. */
static int my_cdev_open(struct inode *inode, struct file *file)
{
    struct my_device_data *data;

    pr_info("Device file is opened.\n");

    /* TODO inode->i_cdev contains our cdev struct, use container_of to obtain a pointer to my_device_data */
    data = container_of(inode->i_cdev, struct my_device_data, cdev);

    /* We set the file's private_data member equal to our device's data so we can access it later
       in the my_cdev_release, my_cdev_read, my_cdev_write functions. */
    file->private_data = data;

    /* TODO Return immediately if access isn't 0, use atomic_cmpxchg */
    if (atomic_cmpxchg(&data->access, 0, 1) != 0)
        return -EBUSY;

    return 0;
}

static int my_cdev_release(struct inode *inode, struct file *file)
{
    struct my_device_data *data = (struct my_device_data *)file->private_data;

    pr_info("Close called!\n");

    /* TODO Reset access variable to 0, use atomic_set */
    atomic_set(&data->access, 0);

    return 0;
}

static ssize_t my_cdev_read(struct file *file, char __user *user_buffer, size_t size, loff_t *offset)
{
    struct my_device_data *data = (struct my_device_data *)file->private_data;
    size_t to_read;

    to_read = (size > data->size - *offset) ? (data->size - *offset) : size;

    pr_info("[READ] Size: %lu Offset: %lld\n", size, *offset);

    /* TODO : Copy data from data->buffer to user buffer. Use copy_to_user. Make sure to include the offset! */
    if (copy_to_user(user_buffer, data->buffer + *offset, to_read))
        return -EFAULT;

    /* move the offset by how many bytes we have read. */
    *offset += to_read;

    return to_read;
}

static ssize_t my_cdev_write(struct file *file, const char __user *user_buffer, size_t size, loff_t *offset)
{
    struct my_device_data *data = (struct my_device_data *)file->private_data;

    pr_info("[WRITE] Size: %lu Offset: %lld\n", size, *offset);

    size = (*offset + size > BUFFER_SIZE) ? (BUFFER_SIZE - *offset) : size;
    /* TODO Copy user_buffer to data->buffer. Use copy_from_user. Make sure to include the offset! */
    if (copy_from_user(data->buffer + *offset, user_buffer, size))
        return -EFAULT;

    /* move the offset by how many bytes we have written. */
    *offset += size;
    data->size = *offset;

    return size;
}

/* This struct will hold pointers to the functions that will be called for each system call that is executed on the device. */

static const struct file_operations my_fops = {
    .owner = THIS_MODULE,
    .open = my_cdev_open,     /* TODO Add open function. */
    .release = my_cdev_release, /* TODO Add release function. */
    .read = my_cdev_read,     /* TODO Add read function */
    .write = my_cdev_write,   /* TODO Add write function */
};

static int my_init(void)
{
    int err;
    int i;

    /* TODO register char device region for MY_MAJOR and NUM_MINORS starting at MY_MINOR.
        Use the register_chrdev_region function */
    err = register_chrdev_region(MKDEV(MY_MAJOR, MY_MINOR), NUM_MINORS, MODULE_NAME);
    if (err != 0) {
        pr_info("Register character device failed.\n");
        return err;
    }

    for (i = 0; i < NUM_MINORS; i++) {
        /* Initializing buffer with MESSAGE string */
        memcpy(devs[i].buffer, MESSAGE, sizeof(MESSAGE));
        devs[i].size = sizeof(MESSAGE);

        /* We set the access variable to 0 using atomic_set */
        atomic_set(&devs[i].access, 0);

        /* TODO init and add cdev to kernel core. Use cdev_init and cdev_add */
        cdev_init(&devs[i].cdev, &my_fops);
        devs[i].cdev.owner = THIS_MODULE;
        devs[i].cdev.ops = &my_fops;

        err = cdev_add(&devs[i].cdev, MKDEV(MY_MAJOR, MY_MINOR + i), 1);
        if (err != 0) {
            pr_info("cdev_add failed.\n");
            unregister_chrdev_region(MKDEV(MY_MAJOR, MY_MINOR), NUM_MINORS);
            return err;
        }
    }

    return 0;
}

static void my_exit(void)
{
    int i;

    for (i = 0; i < NUM_MINORS; i++) {
        /* TODO delete cdev from kernel core using cdev_del */
        cdev_del(&devs[i].cdev);
    }

    /* TODO Unregister char device region, for MY_MAJOR and NUM_MINORS starting at MY_MINOR. Use unregister_chrdev_region */
    unregister_chrdev_region(MKDEV(MY_MAJOR, MY_MINOR), NUM_MINORS);
}

module_init(my_init);
module_exit(my_exit);

