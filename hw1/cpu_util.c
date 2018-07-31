#include <linux/kernel.h>
#include <linux/linkage.h>
#include <linux/fs.h>
#include <linux/file.h>
#include <linux/types.h>
#include <linux/unistd.h>
#include <asm/uaccess.h>
#include <linux/module.h>
#include <linux/stat.h>

struct file *OpenFile(char *path,int flag,int mode){
   struct file *fp;
   fp=filp_open(path, flag, 0);
   if (fp) return fp;
   else return NULL;
}
int ReadFile(struct file *fp,char *buf,int readlen)
{ 
   if (fp->f_op && fp->f_op->read)
      return fp->f_op->read(fp,buf,readlen, &fp->f_pos);
   else
      return -1;
}
int CloseFile(struct file *fp) {
   filp_close(fp,NULL);
   return 0;
}
asmlinkage long sys_cpu_util(void)
{
	int cpu_usage;
	char read_buf[128] = "";
	struct file *fp;
	int total_0 = 0, total_1 = 0;
	int t0 = 0, t1 = 0;
	int cnt = 0, i, tmp = 0;
	mm_segment_t oldfs;
	oldfs = getfs();
	set_fs(KERNEL_DS);



	fp = OpenFile("/proc/stat", O_RDONLY | O_CREAT, 0);
	if(fp != NULL){ 
		tmp = ReadFile(fp, read_buf, sizeof(read_buf));
		if(tmp == -1)
			printk("read fail(1)\n");
	}
	else
		printk("open fail(1)\n");

	CloseFile(fp);

	while(read_buf[cnt++] != ' ');
	++cnt;
	for(i = 0 ; i < 9 ; i++){
		tmp = 0;
		while(read_buf[cnt] >= '0' && read_buf[cnt] <= '9'){
			tmp = tmp * 10 + read_buf[cnt] -'0';
			++cnt;
		}
		if(i == 2) t0 = tmp;
		total_0 += tmp;
		++cnt;
	}

	ssleep(2);

	fp = OpenFile("/proc/stat", O_RDONLY | O_CREAT, 0);
	if(fp != NULL){ 
		tmp = ReadFile(fp, read_buf, sizeof(read_buf));
		if(tmp == -1)
			printk("read fail(2)\n");
	}
	else
		printk("open fail(2)\n");

	CloseFile(fp);


	cnt = 0;
	while(read_buf[cnt++] != ' ');
	++cnt;
	for(i = 0 ; i < 9 ; i++){
		tmp = 0;
		while(read_buf[cnt] >= '0' && read_buf[cnt] <= '9'){
			tmp = tmp * 10 + read_buf[cnt] -'0';
			++cnt;
		}
		if(i == 2) t1 = tmp;
		total_1 += tmp;
		++cnt;
	}
	cpu_usage = (t1 - t0) * 10000;
	cpu_usage /= (total_1 - total_0);
	cnt = cpu_usage / 100;
	tmp = cpu_usage % 100;
	printk("%d.%d%%\n", cnt, tmp);
	set_fs(oldfs);
	return 0;
}
