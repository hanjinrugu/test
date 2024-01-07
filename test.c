#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/mman.h>
#define MEM_DEVICE "/dev/mem"
#define MEM_ADDR 0xfe600008
#define MEM_SIZE 4096UL
#define MEM_MASK (MEM_SIZE - 1)
unsigned int random_array[30] = {
    3116728519, 1294839210, 568540443, 4157096105, 746286261,
    2760624277, 953778306, 685758008, 3597367985, 920508071,
    2169592057, 211029843, 663766675, 3616233776, 3833449360,
    3864968632, 1474388603, 442603818, 880393747, 663067227,
    3636769547, 207895303, 2856975364, 839460437, 989982449,
    3691769444, 517295787, 2310550662, 3406116712, 3398392693};
void devmem_read(unsigned int offset, unsigned int value)
{
  int mem_fd;
  void *mapped_mem;
  off_t target;
  mem_fd = open(MEM_DEVICE, O_RDWR | O_SYNC);
  if (mem_fd == -1)
  {
    perror("Error opening /dev/mem");
    exit(0);
  }
  target = MEM_ADDR | offset;
  mapped_mem = mmap(NULL, MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, target & ~MEM_MASK);
  if (mapped_mem == MAP_FAILED)
  {
    perror("Error mapping memory");
    close(mem_fd);
    exit(0);
  }

  int read_value = *(int *)mapped_mem;
  printf("Value at physical address 0x%x: %x\n", target, read_value);

  if (munmap(mapped_mem, MEM_SIZE) == -1)
  {
    perror("Error unmapping memory");
  }

  close(mem_fd);
}
void devmem_write(unsigned int offset, unsigned int write_value)
{
  int mem_fd;
  void *mapped_mem;
  off_t target;
  mem_fd = open(MEM_DEVICE, O_RDWR | O_SYNC);
  if (mem_fd == -1)
  {
    perror("Error opening /dev/mem");
    exit(0);
  }
  target = MEM_ADDR | offset;
  mapped_mem = mmap(NULL, MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, target & ~MEM_MASK);
  if (mapped_mem == MAP_FAILED)
  {
    perror("Error mapping memory");
    close(mem_fd);
    exit(0);
  }

  *(int *)mapped_mem = write_value;
  printf("Write value at physical address 0x%x: %x\n", MEM_ADDR | offset, *(int *)mapped_mem);

  if (munmap(mapped_mem, MEM_SIZE) == -1)
  {
    perror("Error unmapping memory");
  }

  close(mem_fd);
}
void list_pci_devices()
{
  DIR *dir;
  struct dirent *entry;
  char path[100], vendor[10], device[10];
  FILE *fp;

  dir = opendir("/sys/bus/pci/devices");
  if (dir == NULL)
  {
    perror("Failed to open directory");
    return;
  }

  while ((entry = readdir(dir)) != NULL)
  {
    if (entry->d_name[0] == '.')
      continue;

    snprintf(path, sizeof(path), "/sys/bus/pci/devices/%s/vendor", entry->d_name);
    fp = fopen(path, "r");
    if (fp)
    {
      fgets(vendor, sizeof(vendor), fp);
      fclose(fp);
    }

    snprintf(path, sizeof(path), "/sys/bus/pci/devices/%s/device", entry->d_name);
    fp = fopen(path, "r");
    if (fp)
    {
      fgets(device, sizeof(device), fp);
      fclose(fp);
    }

    printf("Device %s: Vendor ID = %s, Device ID = %s\n", entry->d_name, vendor, device);
  }

  closedir(dir);
}

int main()
{
  char cmd[10];
  while (1)
  {
    printf("p:查看所有pci设备\n");
    printf("t:请输入t对bar空间进行读写测试\n");
    printf("请输入q退出\n");
    scanf("%s", cmd);
    if (strcmp(cmd, "p") == 0)
    {
      list_pci_devices();
    }
    else if (strcmp(cmd, "t") == 0)
    {
      // for (int k = 0; k < 30; k++)
      //{
      devmem_write(0, random_array[0]);
      devmem_read(0, random_array[0]);
      //}
    }
    else if (strcmp(cmd, "q") == 0)
    {
      exit(0);
    }
    else
    {
      printf("输入错误\n");
    }
  }
  return 0;
}
