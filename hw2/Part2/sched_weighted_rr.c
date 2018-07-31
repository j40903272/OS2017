static void update_curr_weighted_rr(struct rq *rq)
{
	struct task_struct *curr = rq->curr;
	u64 delta_exec;
	if (!task_has_weighted_rr_policy(curr))
		return;
	delta_exec = rq->clock - curr->se.exec_start;
	if (unlikely((s64)delta_exec < 0))
		delta_exec = 0;
	schedstat_set(curr->se.exec_max, max(curr->se.exec_max, delta_exec));
	curr->se.sum_exec_runtime += delta_exec;
	curr->se.exec_start = rq->clock;
	cpuacct_charge(curr, delta_exec);
}
static void enqueue_task_weighted_rr(struct rq *rq, struct task_struct *p, int wakeup, bool b)
{
	list_add_tail(&(p->weighted_rr_list_item), &(rq->weighted_rr.queue));
	rq->weighted_rr.nr_running++;
}

static void dequeue_task_weighted_rr(struct rq *rq, struct task_struct *p, int sleep)
{
	update_curr_weighted_rr(rq);
	list_del(&(p->weighted_rr_list_item));
	rq->weighted_rr.nr_running--;
}

static void requeue_task_weighted_rr(struct rq *rq, struct task_struct *p)
{
	list_move_tail(&p->weighted_rr_list_item, &rq->weighted_rr.queue);
}

static void
yield_task_weighted_rr(struct rq *rq)
{
	requeue_task_weighted_rr(rq, rq->curr);
}

static void check_preempt_curr_weighted_rr(struct rq *rq, struct task_struct *p, int wakeflags)
{
}

static struct task_struct *pick_next_task_weighted_rr(struct rq *rq)
{
	struct task_struct *next;
	struct list_head *queue;
	struct weighted_rr_rq *weighted_rr_rq;
	queue = &(rq->weighted_rr).queue;
	weighted_rr_rq = &(rq->weighted_rr);
	if (rq->weighted_rr.nr_running == 0) {
		return NULL;
	} else {
		next = list_first_entry(queue, struct task_struct, weighted_rr_list_item);
		next->se.exec_start = rq->clock;
		return next;
	}
	return next;
}

static void put_prev_task_weighted_rr(struct rq *rq, struct task_struct *p)
{
	update_curr_weighted_rr(rq);
	p->se.exec_start = 0;
}

#ifdef CONFIG_SMP

static struct task_struct *load_balance_start_weighted_rr(void *arg)
{	
	struct rq *rq = arg;
	struct list_head *head, *curr;
	struct task_struct *p;
	head = &rq->weighted_rr.queue;
	curr = head->prev;
	p = list_entry(curr, struct task_struct, weighted_rr_list_item);
	curr = curr->prev;
	rq->weighted_rr.weighted_rr_load_balance_head = head;
	rq->weighted_rr.weighted_rr_load_balance_curr = curr;
	return p;
}

static struct task_struct *load_balance_next_weighted_rr(void *arg)
{
	struct rq *rq = arg;
	struct list_head *curr;
	struct task_struct *p;
	curr = rq->weighted_rr.weighted_rr_load_balance_curr;
	p = list_entry(curr, struct task_struct, weighted_rr_list_item);
	curr = curr->prev;
	rq->weighted_rr.weighted_rr_load_balance_curr = curr;
	return p;
}

static unsigned long
load_balance_weighted_rr(struct rq *this_rq, int this_cpu, struct rq *busiest,
		unsigned long max_load_move,
		struct sched_domain *sd, enum cpu_idle_type idle,
		int *all_pinned, int *this_best_prio)
{
	struct rq_iterator weighted_rr_rq_iterator;
	weighted_rr_rq_iterator.start = load_balance_start_weighted_rr;
	weighted_rr_rq_iterator.next = load_balance_next_weighted_rr;
	weighted_rr_rq_iterator.arg = busiest;
	return balance_tasks(this_rq, this_cpu, busiest, max_load_move, sd,
			     idle, all_pinned, this_best_prio, &weighted_rr_rq_iterator);
}

static int
move_one_task_weighted_rr(struct rq *this_rq, int this_cpu, struct rq *busiest,
		 struct sched_domain *sd, enum cpu_idle_type idle)
{
	struct rq_iterator weighted_rr_rq_iterator;
	weighted_rr_rq_iterator.start = load_balance_start_weighted_rr;
	weighted_rr_rq_iterator.next = load_balance_next_weighted_rr;
	weighted_rr_rq_iterator.arg = busiest;

	return iter_move_one_task(this_rq, this_cpu, busiest, sd, idle,
				  &weighted_rr_rq_iterator);
}
#endif
static void task_tick_weighted_rr(struct rq *rq, struct task_struct *p,int queued)
{
	struct task_struct *curr;
	struct weighted_rr_rq *weighted_rr_rq;
	update_curr_weighted_rr(rq);
	if(p->task_time_slice==0){
		p->task_time_slice=p->weighted_time_slice;
		set_tsk_need_resched(p);
		requeue_task_weighted_rr(rq, p);
	}
	p->task_time_slice--;
	return;	
}
static void set_curr_task_weighted_rr(struct rq *rq)
{
	struct task_struct *p = rq->curr;
	p->se.exec_start = rq->clock;
}
static void switched_to_weighted_rr(struct rq *rq, struct task_struct *p,
			     int running)
{
	if (running)
		resched_task(rq->curr);
	else
		check_preempt_curr(rq, p, 0);
}

static int
select_task_rq_weighted_rr(struct rq *rq, struct task_struct *p, int sd_flag, int flags)
{
	if (sd_flag != SD_BALANCE_WAKE)
		return smp_processor_id();

	return task_cpu(p);
}

const struct sched_class weighted_rr_sched_class = {
	.next			= &idle_sched_class,
	.enqueue_task		= enqueue_task_weighted_rr,
	.dequeue_task		= dequeue_task_weighted_rr,
	.yield_task		= yield_task_weighted_rr,
	.check_preempt_curr	= check_preempt_curr_weighted_rr,
	.pick_next_task		= pick_next_task_weighted_rr,
	.put_prev_task		= put_prev_task_weighted_rr,
#ifdef CONFIG_SMP
	.load_balance		= load_balance_weighted_rr,
	.move_one_task		= move_one_task_weighted_rr,
#endif
	.switched_to  = switched_to_weighted_rr,
	.select_task_rq = select_task_rq_weighted_rr,
	.set_curr_task          = set_curr_task_weighted_rr,
	.task_tick		= (void *)task_tick_weighted_rr,
};
