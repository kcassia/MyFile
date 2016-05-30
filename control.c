#include "sockettest.h"
#include "ph1.h"
#include <efl_extension.h>
#include "connection.h"
#include "timerecord.h"
#include "autoconnection.h"
#include <connection_profile.h>
#include <net_connection.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <net/if.h>
#include <stdbool.h>
#include <stddef.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <sensor.h>
#include <dlog.h>
#include <app_preference.h>
//타이머
#include "timer.h"
#include "log.h"
#include <stdlib.h>
#include <tizen.h>

typedef struct appdata {
   Evas_Object *win;
   Evas_Object *conform;
   Evas_Object *naviframe;
   Evas_Object *label;
   Elm_Object_Item *nf_it;
   //타이머
   Evas_Object *layout;
   Evas_Object *hour_ly;
   Evas_Object *hour_lb;
   Evas_Object *minute_ly;
   Evas_Object *minute_lb;
   Evas_Object *second_ly;
   Evas_Object *second_lb;

   Evas_Object *selected;
   Evas_Object *start_btn;
   Evas_Object *start_btn_ly;
   Ecore_Timer *timer;
   Ecore_Timer *progress_timer;

   char timer_edj_path[MAX_PATH_LEN];
   char timer_control_dot_path[MAX_PATH_LEN];
   int screen_width, screen_height;
   int start;
   int setting_time;
   int ctime;
   int crecord;
   double img_num;
   int hand;

} appdata_s;

sensor_h sensor;
sensor_listener_h listener;

void sensor_test();

int rotary_count_left = 1;
int rotary_count_right = 1;
char ip[16]="";
connection_h connection;
int sensor_start =0;
#define MAXLINE 1024

char start_msg[] = "START";
char Go_msg[] = "GO";
char Back_msg[] = "BACK";
char ECS_msg[] = "ESC";
char F5_msg[] = "F5";

static void layout_back_block (void *data, Evas_Object *obj, void *event_info)
{

}


int  inputIP(char input_ip[16]){
   strcpy(ip, input_ip);
   //dlog_print(DLOG_INFO, "IP", ip);
   if(ip[0] != '.'){
      return 1;
   }else{
      return -1;
   }

}

void initiate(){
   int error_code;

   error_code = connection_create(&connection);
   if (error_code != CONNECTION_ERROR_NONE)
      return;

   char *ip_addr = NULL;
   error_code = connection_get_ip_address(connection,CONNECTION_ADDRESS_FAMILY_IPV4, &ip_addr);
   if (error_code == CONNECTION_ERROR_NONE) {
      free(ip_addr);
   }
}

int findIp(){

   struct sockaddr_in server_addr;
   struct sockaddr_in client_addr;
   int sockfd = -1;
   unsigned int client_addr_size;
   int recvlen=0;

   char   buff_rcv[MAXLINE];

   client_addr_size = sizeof(client_addr);

   memset(&server_addr, 0, sizeof(server_addr));
   server_addr.sin_family = AF_INET;
   server_addr.sin_port = htons(7777);
   server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

   //Socket Creation
   if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
      //dlog_print(DLOG_INFO, "SOCKETTEST", "failed to create UDP socket");
   } else {
      //dlog_print(DLOG_INFO, "SOCKETTEST", "IP : %s", ip);
      //dlog_print(DLOG_INFO, "SOCKETTEST", "Success to create UDP socket");
   }

   if (-1 == bind(sockfd, (struct sockaddr*) &server_addr,sizeof(server_addr))) {
      //dlog_print(DLOG_INFO, "SOCKETTEST", "Bind Error");
   } else {
      //dlog_print(DLOG_INFO, "SOCKETTEST", "Bind Success");
   }

   //dlog_print(DLOG_INFO, "SOCKETTEST", "Waiting DATA...");

   /* recvfrom 시간 제한 설정 */
   struct timeval timeout = { 9, 0 };
   setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char*) &timeout,sizeof(struct timeval));

   /* UDP 브로드 캐스트 수신 */
   recvlen = recvfrom(sockfd, buff_rcv, MAXLINE, 0, (struct sockaddr*) &client_addr,&client_addr_size);
   //dlog_print(DLOG_INFO, "SOCKETTEST", "Receive Data : %s", buff_rcv);

   strcpy(ip, buff_rcv);
   //dlog_print(DLOG_INFO, "SOCKETTEST", "Changed IP : %s", ip);
   close(sockfd);

   if(recvlen > 7)
      return 1;
   else
      return 0;
}


void remote_control_cb(int index) {

   struct sockaddr_in server_addr;
   int sockfd = -1;

   //Socket 생성
   memset(&server_addr, 0, sizeof(server_addr));
   server_addr.sin_family = AF_INET;
   server_addr.sin_port = htons(7777);
   server_addr.sin_addr.s_addr = inet_addr(ip);

   //Socket Creation
   if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      //dlog_print(DLOG_INFO, "SOCKETTEST", "failed to create socket");
      return;
   } else {
      //dlog_print(DLOG_INFO, "SOCKETTEST", "Success to create socket");
   }

   //Connection
   if (connect(sockfd, (struct sockaddr*) &server_addr, sizeof(server_addr))< 0) {
      //dlog_print(DLOG_INFO, "SOCKETTEST", "socket connect error: %s\n",strerror(errno));
      close(sockfd);
   } else {
      //dlog_print(DLOG_INFO, "SOCKETTEST", "Success to socket connection");
   }

   int count =0;


   //Write
   switch(index){

   case 1:
      if ((count = write(sockfd, start_msg, 5)) < 0) {
         //dlog_print(DLOG_INFO, "SOCKETTEST", "write() error: %s\n",strerror(errno));
         close(sockfd);
      } else {
         // Create a thread
      }
      close(sockfd);
      break;

   case 2:

      if ((count = write(sockfd, Go_msg, 2)) < 0)
      {
         //dlog_print(DLOG_INFO, "SOCKETTEST", "write() error: %s\n", strerror(errno));
         close(sockfd);
      }else{
         //dlog_print(DLOG_INFO, "SOCKETTEST", "write() 성공\n");
      }
      close(sockfd);
      break;

   case 3:

      if ((count = write(sockfd, Back_msg, 4)) < 0)
      {
         //dlog_print(DLOG_INFO, "SOCKETTEST", "write() error: %s\n", strerror(errno));
         close(sockfd);
      }else{
         //dlog_print(DLOG_INFO, "SOCKETTEST", "write() 성공\n");
      }
      close(sockfd);
      break;
   }

}

void start_cb () {

   struct sockaddr_in server_addr;
   int sockfd = -1;

   if(sensor_start == 0){
      sensor_start = 1;
      sensor_test();
   }

   //Socket 생성
   memset(&server_addr, 0, sizeof(server_addr));
   server_addr.sin_family = AF_INET;
   server_addr.sin_port = htons(7777);
   server_addr.sin_addr.s_addr = inet_addr(ip);

   //Socket Creation
   if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      //dlog_print(DLOG_INFO, "SOCKETTEST", "failed to create socket");
      return;
   } else {
      //dlog_print(DLOG_INFO, "SOCKETTEST", "Success to create socket");
   }

   //Connection
   if (connect(sockfd, (struct sockaddr*) &server_addr, sizeof(server_addr))< 0) {
      //dlog_print(DLOG_INFO, "SOCKETTEST", "socket connect error: %s\n",strerror(errno));
      close(sockfd);
   } else {
      //dlog_print(DLOG_INFO, "SOCKETTEST", "Success to socket connection");
   }

   int count =0;


   //Write
   if ((count = write(sockfd, F5_msg, 2)) < 0)
   {
      //dlog_print(DLOG_INFO, "SOCKETTEST", "write() error: %s\n", strerror(errno));
      close(sockfd);
   }else{
      //dlog_print(DLOG_INFO, "SOCKETTEST", "write() 성공\n");
   }

   close(sockfd);
}

void esc_cb() {

   struct sockaddr_in server_addr;
   int sockfd = -1;

   //Sensing stop
   sensor_listener_unset_event_cb(listener);
   sensor_listener_stop(listener);
   sensor_start = 0;

   //Socket 생성
   memset(&server_addr, 0, sizeof(server_addr));
   server_addr.sin_family = AF_INET;
   server_addr.sin_port = htons(7777);
   server_addr.sin_addr.s_addr = inet_addr(ip);

   //Socket Creation
   if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      //dlog_print(DLOG_INFO, "SOCKETTEST", "failed to create socket");
      return;
   } else {
      //dlog_print(DLOG_INFO, "SOCKETTEST", "Success to create socket");
   }

   //Connection
   if (connect(sockfd, (struct sockaddr*) &server_addr, sizeof(server_addr))< 0) {
      //dlog_print(DLOG_INFO, "SOCKETTEST", "socket connect error: %s\n",strerror(errno));
      close(sockfd);
   } else {
      //dlog_print(DLOG_INFO, "SOCKETTEST", "Success to socket connection");
   }

   int count =0;


   //Write
   if ((count = write(sockfd, ECS_msg, 3)) < 0)
   {
      //dlog_print(DLOG_INFO, "SOCKETTEST", "write() error: %s\n", strerror(errno));
      close(sockfd);
   }else{
      //dlog_print(DLOG_INFO, "SOCKETTEST", "write() 성공\n");
   }

   close(sockfd);
}


Eina_Bool _rotary_handler_cb(void *data, Eext_Rotary_Event_Info *ev) {

   if (ev->direction == EEXT_ROTARY_DIRECTION_CLOCKWISE && rotary_count_left == 2) {
      remote_control_cb(2);
      rotary_count_left = 1;
      rotary_count_right = 1;
      //dlog_print(DLOG_INFO, "ROTARTY", "배젤링 앞으로");
   }else if(ev->direction == EEXT_ROTARY_DIRECTION_CLOCKWISE && rotary_count_left == 1){
	   rotary_count_left++;
	   rotary_count_right = 1;
   }else if (rotary_count_right == 2) {
      remote_control_cb(3);
      rotary_count_right = 1;
      rotary_count_left = 1;
      //dlog_print(DLOG_INFO, "ROTARTY", "배젤링 뒤로");
   }else {
	   rotary_count_right++;
	   rotary_count_left = 1;
   }

   return EINA_FALSE;
}

void sensor_event_callback_left (sensor_h sensor, sensor_event_s *event, void *data)
{

   // float x, y, z;
   float x;
   int flag = 0;
   sensor_type_e type;
   sensor_get_type(sensor, &type);
   if(type == SENSOR_GYROSCOPE)
   {
      dlog_print(DLOG_INFO, "SENSOR", "%f, %f, %f", event->values[0], event->values[1], event->values[2]);
      x = event->values[0];
      //y = event->values[1];
      //z = event->values[2];
      if (x < -1000.0 && flag == 0)
      {
         //            if (z > -9 && z < 2)
         //               if (x > -18 && x < -6)
         //               {
         //dlog_print(DLOG_INFO, "MOTION", "%f, %f, %f", event->values[0], event->values[1], event->values[2]);
         //dlog_print(DLOG_INFO, "SENSOR", "왼손 모션 앞으로 ");
         remote_control_cb(2);
         flag = 1;
         //               }
      }else if (x > 1000)
      {
         flag = 1;
      }
      else
         flag = 0;
   }

}
void sensor_event_callback_right (sensor_h sensor, sensor_event_s *event, void *data)
{

   // float x, y, z;
   float x;
   int flag = 0;
   sensor_type_e type;
   sensor_get_type(sensor, &type);
   if(type == SENSOR_GYROSCOPE)
   {
      dlog_print(DLOG_INFO, "SENSOR", "%f, %f, %f", event->values[0], event->values[1], event->values[2]);
      x = event->values[0];
      //y = event->values[1];
      //z = event->values[2];
      //if (x > 600.0 && flag == 0)
      if (x < -900.0 && flag == 0)
      {
         //            if (z > -9 && z < 2)
         //               if (x > -18 && x < -6)
         //               {
         //dlog_print(DLOG_INFO, "MOTION", "%f, %f, %f", event->values[0], event->values[1], event->values[2]);
         //dlog_print(DLOG_INFO, "SENSOR", "오른손 모션 앞으로 ");
         remote_control_cb(2);
         flag = 1;
         //               }
      }else if (x > 900)
      //else if (x < -1000)
         flag = 1;
      else
         flag = 0;
   }

}

void sensor_test(){
   //Starting sensor listener
   sensor_type_e type = SENSOR_GYROSCOPE;
   bool existing = false;

   if (sensor_get_default_sensor(type, &sensor) == SENSOR_ERROR_NONE)
   {
	   if ((preference_is_existing("hand", &existing) == 0) && existing) {
	         preference_get_int("hand", &hand);
	   }

      if (hand == 0)
      {
         if (sensor_create_listener(sensor, &listener) == SENSOR_ERROR_NONE
                  && sensor_listener_set_event_cb(listener, 100, sensor_event_callback_left, NULL) == SENSOR_ERROR_NONE)
            {
               if (sensor_listener_start(listener) == SENSOR_ERROR_NONE)
               {
                  sensor_listener_set_option(listener, SENSOR_OPTION_ALWAYS_ON);
               }
            }
      }else if (hand == 1)
      {
         if (sensor_create_listener(sensor, &listener) == SENSOR_ERROR_NONE
                  && sensor_listener_set_event_cb(listener, 100, sensor_event_callback_right, NULL) == SENSOR_ERROR_NONE)
            {
               if (sensor_listener_start(listener) == SENSOR_ERROR_NONE)
               {
                  sensor_listener_set_option(listener, SENSOR_OPTION_ALWAYS_ON);
               }
            }
      }
   }
}




//rotary.c로부터
static char *_rotary_get_plus_value(appdata_s *ad)
{
   const char *pre_change_num = NULL;
   int post_change_num = 0;
   char buf[256] = {0, };
   char *val = NULL;

   pre_change_num = elm_object_part_text_get(ad->selected, "val");
   post_change_num = atoi(pre_change_num);

   if (post_change_num == 60) {
      val = strdup(pre_change_num);
      return val;
   }

   post_change_num = post_change_num + 1;

   if (post_change_num == 0) {
      snprintf(buf, sizeof(buf), "0%d", post_change_num);
      val = strdup(buf);
   } else {
      snprintf(buf, sizeof(buf), "%d", post_change_num);
      val = strdup(buf);
   }

   return val;
}

static char *_rotary_get_minus_value(appdata_s *ad)
{
   const char *pre_change_num = NULL;
   int post_change_num = 0;
   char buf[256] = {0, };
   char *val = NULL;

   pre_change_num = elm_object_part_text_get(ad->selected, "val");
   post_change_num = atoi(pre_change_num);

   if (post_change_num == 0) {
      val = strdup(pre_change_num);
      return val;
   }

   post_change_num = post_change_num - 1;

   if (post_change_num == 0) {
      snprintf(buf, sizeof(buf), "0%d", post_change_num);
      val = strdup(buf);
   } else {
      snprintf(buf, sizeof(buf), "%d", post_change_num);
      val = strdup(buf);
   }

   return val;
}

Eina_Bool _rotary_cb(void *data, Evas_Object *obj, Eext_Rotary_Event_Info *rotary_info)
{
   char *val = NULL;
   appdata_s *ad = data;

   retv_if(!ad, ECORE_CALLBACK_PASS_ON);
   retv_if(!ad->selected, ECORE_CALLBACK_PASS_ON);
   retv_if(ad->timer, ECORE_CALLBACK_PASS_ON);

   if (rotary_info->direction == EEXT_ROTARY_DIRECTION_CLOCKWISE) {
      val = _rotary_get_plus_value(ad);
      elm_object_part_text_set(ad->selected, "val", val);
   } else {
      val = _rotary_get_minus_value(ad);
      elm_object_part_text_set(ad->selected, "val", val);
   }

   free(val);

   return ECORE_CALLBACK_PASS_ON;
}


void rotary_init(appdata_s *ad)
{
   //베젤링 연동용
   //rotary events를 감지하고 necessary callback functions을 추가
   eext_rotary_object_event_callback_add(ad->layout, _rotary_cb, ad);
   eext_rotary_object_event_activated_set(ad->layout, EINA_TRUE);

}

void rotary_destroy(appdata_s *ad)
{
   eext_rotary_object_event_callback_del(ad->layout, _rotary_cb);
}


//digital.c로부터
static void _hour_selected_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   appdata_s *ad = data;

   ad->selected = obj;

   elm_object_signal_emit(obj, "hour_selected", "hour");
   elm_object_signal_emit(ad->minute_ly, "minute_deselected", "minute");
   elm_object_signal_emit(ad->second_ly, "second_deselected", "second");
}

static void _minute_selected_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   appdata_s *ad = data;

   ad->selected = obj;

   elm_object_signal_emit(obj, "minute_selected", "minute");
   elm_object_signal_emit(ad->hour_ly, "hour_deselected", "hour");
   elm_object_signal_emit(ad->second_ly, "second_deselected", "second");
}

static void _second_selected_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   appdata_s *ad = data;
   ad->selected = obj;

   elm_object_signal_emit(obj, "second_selected", "second");
   elm_object_signal_emit(ad->hour_ly, "hour_deselected", "hour");
   elm_object_signal_emit(ad->minute_ly, "minute_deselected", "minute");
}


//발표 제한 시간 입력 받기 위한 디지털 시계 생성
void digital_create(appdata_s *ad)
{
   ad->hour_ly = elm_layout_add(ad->layout);
   ret_if(!ad->hour_ly);
   elm_layout_file_set(ad->hour_ly, ad->timer_edj_path, "hour");
   //마우스로 선택되면 선택된 시간축에 시그널을 줌
   evas_object_event_callback_add(ad->hour_ly, EVAS_CALLBACK_MOUSE_DOWN, _hour_selected_cb, ad);
   elm_object_part_content_set(ad->layout, "hour", ad->hour_ly);
   //화면 각 시간 항목 레이어에 값 넘어주는 방법
   elm_object_part_text_set(ad->hour_ly, "val", "00");
   evas_object_show(ad->hour_ly);

   ad->minute_ly = elm_layout_add(ad->layout);
   ret_if(!ad->minute_ly);
   elm_layout_file_set(ad->minute_ly, ad->timer_edj_path, "minute");
   evas_object_event_callback_add(ad->minute_ly, EVAS_CALLBACK_MOUSE_DOWN, _minute_selected_cb, ad);
   elm_object_part_content_set(ad->layout, "minute", ad->minute_ly);
   elm_object_part_text_set(ad->minute_ly, "val", "00");
   evas_object_show(ad->minute_ly);

   ad->second_ly = elm_layout_add(ad->layout);
   ret_if(!ad->second_ly);
   elm_layout_file_set(ad->second_ly, ad->timer_edj_path, "second");
   evas_object_event_callback_add(ad->second_ly, EVAS_CALLBACK_MOUSE_DOWN, _second_selected_cb, ad);
   elm_object_part_content_set(ad->layout, "second", ad->second_ly);
   elm_object_part_text_set(ad->second_ly, "val", "00");
   evas_object_show(ad->second_ly);

}

void main_get_resource_path(appdata_s *ad)
{
   static char *res_path_buff = NULL;

   if (res_path_buff == NULL) {
      res_path_buff = app_get_resource_path();
   }

   snprintf(ad->timer_edj_path, MAX_PATH_LEN, "%s%s", res_path_buff, TIMER_EDJ);
   snprintf(ad->timer_control_dot_path, MAX_PATH_LEN, "%s%s", res_path_buff, CONTROL_DOT);

   _D("Timer edj path: %s", ad->timer_edj_path);
   _D("Timer control dot path: %s", ad->timer_control_dot_path);
}

//calculates the quantity of time
//입력 받은 시간값을 초단위로 환산해서 INT 값으로 리턴
static int _calc_setting_time(appdata_s *ad)
{
   int post_second_time = 0;
   int post_minute_time = 0;
   int post_hour_time = 0;
   int setting_time = 0;
   const char *pre_second_time = NULL;
   const char *pre_minute_time = NULL;
   const char *pre_hour_time = NULL;

   //string으로 된 value 값을
   pre_second_time = elm_object_part_text_get(ad->second_ly, "val");
   //int 값으로 바꿔서 전환
   post_second_time = atoi(pre_second_time);
   pre_minute_time = elm_object_part_text_get(ad->minute_ly, "val");
   post_minute_time = atoi(pre_minute_time);
   pre_hour_time = elm_object_part_text_get(ad->hour_ly, "val");
   post_hour_time = atoi(pre_hour_time);
   //초단위로 환산
   setting_time = post_second_time + (post_minute_time*60) + (post_hour_time*3600);
   _D("Setting time is %d", setting_time);

   return setting_time;
}

//스타트 후에 1초마다 초값 올리는 타이머
static Eina_Bool _timer_start_cb(void *data)
{
   appdata_s *ad = data;
   int post_second_time = 0;
   int post_minute_time = 0;
   int post_hour_time = 0;
   char *hour_val = NULL;
   char *minute_val = NULL;
   char *second_val = NULL;
   char buf[256] = { 0, };
   const char *pre_second_time = NULL;
   const char *pre_minute_time = NULL;
   const char *pre_hour_time = NULL;

   //스트링으로 되어 있는 타임 값을 int 값으로 변환
   pre_second_time = elm_object_part_text_get(ad->second_ly, "val");
   post_second_time = atoi(pre_second_time);
   pre_minute_time = elm_object_part_text_get(ad->minute_ly, "val");
   post_minute_time = atoi(pre_minute_time);
   pre_hour_time = elm_object_part_text_get(ad->hour_ly, "val");
   post_hour_time = atoi(pre_hour_time);

   //1초 상승시킴
   if (post_second_time == 59)
   {
      if(post_minute_time == 59)
      {
         post_second_time = 0;
         post_minute_time = 0;
         post_hour_time++;
      }
      else
      {
         post_second_time = 0;
         post_minute_time++;
      }
   }
   else
      post_second_time++;

   //레이어에 처넣기 전에 전처리
   snprintf(buf, sizeof(buf), "%02d", post_hour_time);
   hour_val = strdup(buf);
   snprintf(buf, sizeof(buf), "%02d", post_minute_time);
   minute_val = strdup(buf);
   snprintf(buf, sizeof(buf), "%02d", post_second_time);
   second_val = strdup(buf);
   //레이어에 증가된 시간 삽입
   elm_object_part_text_set(ad->hour_ly, "val", hour_val);
   elm_object_part_text_set(ad->minute_ly, "val", minute_val);
   elm_object_part_text_set(ad->second_ly, "val", second_val);

   ad->ctime++; //초값과 함께 증가 제한 시간이랑 상관없는 현재 시간값을 담을 변수

   free(hour_val);
   free(minute_val);
   free(second_val);

   return ECORE_CALLBACK_RENEW;
}

static Eina_Bool _progress_start_cb(void *data)
{
   appdata_s *ad = data;
   char buf[256] = { 0, };
   char *sig_num = NULL;
   int temp = 0;

   if (ad->img_num == 0) {
      elm_object_signal_emit(ad->layout, "timer_end", "progress");
      ecore_timer_del(ad->progress_timer);
      ad->progress_timer = NULL;

      //return ECORE_CALLBACK_CANCEL;
      return ECORE_CALLBACK_RENEW;
   }

   temp = (int)ad->img_num;
   snprintf(buf, sizeof(buf), "starting_%d", temp);
   sig_num = strdup(buf);

   elm_object_signal_emit(ad->layout, sig_num, "progress");

   ad->img_num--;

   free(sig_num);

   return ECORE_CALLBACK_RENEW;
}


static void _reset_clicked_cb(appdata_s *ad)
{
   ad->setting_time = 0;
   ad->crecord = ad->ctime;
   ecore_timer_del(ad->timer);
   ad->timer = NULL;
   ecore_timer_del(ad->progress_timer);
   ad->progress_timer = NULL;

   //합치면서 추가
   eext_rotary_event_handler_del(_rotary_handler_cb); //베젤링 페이지 넘김 핸들러 죽이는 코드
   esc_cb(); //통신 정리
   elm_object_part_text_set(ad->start_btn_ly, "btn_txt", "START");

   elm_object_part_text_set(ad->hour_ly, "val", "00");
   elm_object_part_text_set(ad->minute_ly, "val", "00");
   elm_object_part_text_set(ad->second_ly, "val", "00");
   elm_object_signal_emit(ad->start_btn_ly, "timer_end", "btn");
   elm_object_signal_emit(ad->layout, "timer_end", "bg");
   elm_object_signal_emit(ad->layout, "timer_end", "progress");

   int get_num = 0;
   char set_num[2] = "1";
   bool existing = false;

   if ((preference_is_existing("numrecord", &existing) == 0) && existing) {
      preference_get_int("numrecord", &get_num);
      if(get_num == 5)
      {
         for(int i=5; i>1; i--)
         {
            snprintf(set_num, sizeof(set_num), "%d", i-1);
            preference_get_int(set_num, &get_num);
            snprintf(set_num, sizeof(set_num), "%d", i);
            preference_set_int(set_num, get_num);
         }
         snprintf(set_num, sizeof(set_num), "%d", 1);
      }
      else
      {
         int temp_num = get_num;
         for (int i = temp_num; i > 1; i--) {
            snprintf(set_num, sizeof(set_num), "%d", i - 1);
            preference_get_int(set_num, &get_num);
            snprintf(set_num, sizeof(set_num), "%d", i);
            preference_set_int(set_num, get_num);
         }
         snprintf(set_num, sizeof(set_num), "%d", 1);
      }
   }

   preference_set_int(set_num, ad->crecord);
   //dlog_print(DLOG_INFO, "BUG", "set_num : %s, ad->crecord : %d", set_num, ad->crecord);

   //dlog_print(DLOG_INFO, "ResetClicked", "Reset_Clicked_cb / num,time %s %d", set_num, ad->crecord);

   //백버튼 블락용 함수 죽임
   eext_object_event_callback_del (ad->layout, EEXT_CALLBACK_BACK, layout_back_block);
   elm_naviframe_item_pop(ad->naviframe); //layout 팝업

}

void save_setting_time(int settingTime){
   //dlog_print(DLOG_INFO, "ResetClicked", "save_setting _time  : %d", settingTime);
   bool existing = false;

       int count = 0;
       char sett[4] = "";

//      if ((preference_is_existing("numrecord", &existing) == 0) && existing) {
//            preference_get_int("numrecord", &count);
//            count++;
//            count*=100;
//            snprintf(sett, sizeof(sett), "%d", count);
//            preference_set_int(sett, settingTime);
//      }else{
//         preference_set_int("100", settingTime);
//      }
      if ((preference_is_existing("numrecord", &existing) == 0) && existing){}
      else
            preference_set_int("numrecord", 0);

      if ((preference_is_existing("numrecord", &existing) == 0) && existing) {
         preference_get_int("numrecord", &count);
         if(count == 5)
          {
          for(int i=5; i>1; i--)
           {
              snprintf(sett, sizeof(sett), "%d", (i-1)*100);
              preference_get_int(sett, &count);
              snprintf(sett, sizeof(sett), "%d", i*100);
              preference_set_int(sett, count);
           }
           snprintf(sett, sizeof(sett), "%d", 100);
          }
          else
          {

            //           preference_get_int("numrecord", &count);
            //           int temp_num = count;
            //           temp_num++;
            //           temp_num *= 100;
            //           snprintf(sett, sizeof(sett), "%d", temp_num);
            //
            //           preference_set_int("numrecord", temp_num);
                       int temp_num = count + 1;
                       for(int i=temp_num; i>1; i--)
                       {
                          snprintf(sett, sizeof(sett), "%d", (i-1)*100);
                          preference_get_int(sett, &count);
                          snprintf(sett, sizeof(sett), "%d", i*100);
                          preference_set_int(sett, count);
                       }
                        snprintf(sett, sizeof(sett), "%d", 1*100);
                    preference_set_int("numrecord", temp_num);
          }
        }

      preference_set_int(sett, settingTime);
      //dlog_print(DLOG_INFO, "BUG", "sett : %s, settiingTime : %d", sett, settingTime);
}

//타이머 동작 및  displays the total time
static void _start_clicked_cb(void *data, Evas_Object *obj, void *event_info)
{
   //dlog_print(DLOG_INFO, "PTESTResetClicked", "1start_clicked" );
   appdata_s *ad = data;
   double progress_count = 0;
   //back 버튼 블락용 가짜 콜백 함수 추가


   //dlog_print(DLOG_INFO, "PTESTResetClicked", "2before if" );
   if (ad->timer)  //Ecore_Timer *timer;
   {
      //dlog_print(DLOG_INFO, "PTESTResetClicked", "3adtimer if" );
      _reset_clicked_cb(ad);
      return;
   }

   //dlog_print(DLOG_INFO, "PTESTResetClicked", "4if" );
   //입력한 시간 초로 환산
   ad->setting_time = _calc_setting_time(ad);
   //입력한 시간이 0초 이상이면 레이아웃 교체
   if (ad->setting_time != 0)
   {

     save_setting_time(ad->setting_time);

      eext_object_event_callback_add(ad->layout, EEXT_CALLBACK_BACK, layout_back_block, NULL);
      rotary_destroy (ad); //기존 시간 입력용 베젤링 죽임.
      elm_object_part_text_set(ad->start_btn_ly, "btn_txt", "END");
   }
   ad->ctime = 0;
   elm_object_part_text_set(ad->hour_ly, "val", "00");
   elm_object_part_text_set(ad->minute_ly, "val", "00");
   elm_object_part_text_set(ad->second_ly, "val", "00");
   ret_if(!ad->setting_time);

   //기능합칠 때 추가
   start_cb ();  //소켓과 시작 센서 테스트
   eext_rotary_event_handler_add(_rotary_handler_cb, NULL); //베젤링 페이지 넘기기

   ad->img_num = 60;

   progress_count = (ad->setting_time/ad->img_num);
   ad->selected = NULL;
   // Make the digital clock unclickable
   elm_object_signal_emit(ad->hour_ly, "hour_deselected", "hour");
   elm_object_signal_emit(ad->minute_ly, "minute_deselected", "minute");
   elm_object_signal_emit(ad->second_ly, "second_deselected", "second");

   elm_object_signal_emit(ad->start_btn_ly, "timer_start", "btn");
   elm_object_signal_emit(ad->layout, "timer_start", "bg");

   //Reduce 1 second every second
   //ecore_timer_add : Creates a timer to call the given function in the given period of time.
   //(타이머시간초단위로, given function, Data to pass to func)
   //func will be called every in seconds
   ad->timer = ecore_timer_add(1.0f, _timer_start_cb, ad);
   ret_if(!ad->timer);
   ad->progress_timer = ecore_timer_add(progress_count, _progress_start_cb, ad);
   ret_if(!ad->progress_timer);
   _progress_start_cb(ad);
}

//스타트 버튼을 생성하고 이미지를 넣어줌
static void start_btn_create(appdata_s *ad)
{
   ad->start_btn_ly = elm_layout_add(ad->layout);
   ret_if(!ad->start_btn_ly);
   elm_layout_file_set(ad->start_btn_ly, ad->timer_edj_path, "start_btn");
   elm_object_part_content_set(ad->layout, "start_button", ad->start_btn_ly);
   evas_object_show(ad->start_btn_ly);
   ad->start_btn = elm_image_add(ad->start_btn_ly);
   ret_if(!ad->start_btn);
   evas_object_size_hint_weight_set(ad->start_btn, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(ad->start_btn, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_object_part_content_set(ad->start_btn_ly, "btn", ad->start_btn);
   //text 수정 btn_txt가 버튼 밑에 text
   elm_object_part_text_set(ad->start_btn_ly, "btn_txt", "START");
   //elm_object_part_content_set(ad->start_btn_ly, "btn_img", ad->start_btn);

   /*btn_img
   현재 이미지 파일 교체 실험을 위해 timer.edc 파일에 보면
   image: "unknown.png" COMP; 문장을 이미지 콜렙션에 추가하고
   start_btn 세컨드 이미지로 normal: "unknown.png" 수정
    */
   ad->timer=NULL;
   evas_object_smart_callback_add(ad->start_btn, "clicked", _start_clicked_cb, ad);
}


void create_third_page(void *data, Evas_Object *obj, void *event_info)
{
   struct appdata *ad = data;
   Elm_Object_Item *nf_it = NULL;
   ad->selected = NULL;
   //레이아웃 잡아주고
   ad->layout = elm_layout_add(ad->naviframe);
   //리소스 패스 잡는다.
   main_get_resource_path(ad);
   //위 함수 결과 : ad->timer_edj_path == app_get_resource_path() + /edje/timer.edj
   elm_layout_file_set(ad->layout, ad->timer_edj_path, GRP_MAIN);
   evas_object_size_hint_weight_set(ad->layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   //elm_object_content_set(ad->naviframe, ad->layout); //만악의 근원. 극악 버그의 원흉

   //화면 가운데 버튼 생성
   start_btn_create(ad);
   //시계 레이아웃 생성
   digital_create(ad);
   //베젤링
   rotary_init(ad);

   evas_object_show(ad->layout);

   nf_it = elm_naviframe_item_push(ad->naviframe,NULL, NULL, NULL, ad->layout, NULL);
   elm_naviframe_item_title_enabled_set(nf_it, EINA_FALSE, EINA_FALSE);
}
