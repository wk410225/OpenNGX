/*
Navicat MySQL Data Transfer

Source Server         : 192.168.43.12
Source Server Version : 80041
Source Host           : 192.168.43.12:3306
Source Database       : ry

Target Server Type    : MYSQL
Target Server Version : 80041
File Encoding         : 65001

Date: 2025-09-03 18:48:27
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for tbl_srv_inf
-- ----------------------------
DROP TABLE IF EXISTS `tbl_srv_inf`;
CREATE TABLE `tbl_srv_inf` (
  `USAGE_KEY` int NOT NULL,
  `SRV_ID` char(4) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin NOT NULL,
  `SRV_NAME` char(20) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin NOT NULL,
  `MSQ_INT_ID` char(4) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin NOT NULL,
  `SRV_NUM` char(2) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin NOT NULL,
  `RELATE_SRV_ID` varchar(2048) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin DEFAULT NULL,
  `SRV_DSP` char(64) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin DEFAULT NULL,
  UNIQUE KEY `PK_TBL_SRV_INF` (`USAGE_KEY`,`SRV_ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_bin COMMENT='tbl srv info';
--select * from tbl_srv_inf;
delete from tbl_srv_inf;
insert into tbl_srv_inf (USAGE_KEY, SRV_ID, SRV_NAME, MSQ_INT_ID, SRV_NUM, RELATE_SRV_ID, SRV_DSP) values (0,'1106','SwtRY','1106','01','2810','若依处理');
insert into tbl_srv_inf (USAGE_KEY, SRV_ID, SRV_NAME, MSQ_INT_ID, SRV_NUM, RELATE_SRV_ID, SRV_DSP) values (0,'2810','CommHTTP','2810','01','1106','http server');
/*
Navicat MySQL Data Transfer

Source Server         : 192.168.43.12
Source Server Version : 80041
Source Host           : 192.168.43.12:3306
Source Database       : ry

Target Server Type    : MYSQL
Target Server Version : 80041
File Encoding         : 65001

Date: 2025-09-03 18:48:38
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for tbl_srv_param
-- ----------------------------
DROP TABLE IF EXISTS `tbl_srv_param`;
CREATE TABLE `tbl_srv_param` (
  `USAGE_KEY` int NOT NULL,
  `SRV_ID` char(4) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin NOT NULL,
  `PARAM_USAGE` char(1) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin NOT NULL,
  `PARAM_INDEX` char(2) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin NOT NULL,
  `PARAM_DATA` char(64) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin NOT NULL,
  `PARAM_DSP` char(64) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin DEFAULT NULL,
  UNIQUE KEY `PK_TBL_SRV_PARAM` (`USAGE_KEY`,`SRV_ID`,`PARAM_USAGE`,`PARAM_INDEX`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_bin COMMENT='tbl srv param info';
--select * from tbl_srv_param where srv_id in ('1106','2810');
delete from tbl_srv_param where srv_id in ('1106','2810');
insert into tbl_srv_param (USAGE_KEY, SRV_ID, PARAM_USAGE, PARAM_INDEX, PARAM_DATA, PARAM_DSP) values (0,'1106','0','01','SwtRY','');
insert into tbl_srv_param (USAGE_KEY, SRV_ID, PARAM_USAGE, PARAM_INDEX, PARAM_DATA, PARAM_DSP) values (0,'1106','1','01','LOG_MODE=5','');
insert into tbl_srv_param (USAGE_KEY, SRV_ID, PARAM_USAGE, PARAM_INDEX, PARAM_DATA, PARAM_DSP) values (0,'1106','1','03','LOG_SWITCH_MODE=1','');
insert into tbl_srv_param (USAGE_KEY, SRV_ID, PARAM_USAGE, PARAM_INDEX, PARAM_DATA, PARAM_DSP) values (0,'1106','1','04','LOG_SIZE=20','');
insert into tbl_srv_param (USAGE_KEY, SRV_ID, PARAM_USAGE, PARAM_INDEX, PARAM_DATA, PARAM_DSP) values (0,'1106','1','05','MSG_COMPRESS_FLAG=N','');
insert into tbl_srv_param (USAGE_KEY, SRV_ID, PARAM_USAGE, PARAM_INDEX, PARAM_DATA, PARAM_DSP) values (0,'1106','1','06','MSQ_NUM_MAX=1000','');
insert into tbl_srv_param (USAGE_KEY, SRV_ID, PARAM_USAGE, PARAM_INDEX, PARAM_DATA, PARAM_DSP) values (0,'1106','1','07','SRV_USAGE_KEY=0','');
insert into tbl_srv_param (USAGE_KEY, SRV_ID, PARAM_USAGE, PARAM_INDEX, PARAM_DATA, PARAM_DSP) values (0,'1106','1','08','LOG_FILE_PATH=$HOME/log/','');
insert into tbl_srv_param (USAGE_KEY, SRV_ID, PARAM_USAGE, PARAM_INDEX, PARAM_DATA, PARAM_DSP) values (0,'2810','0','01','CommHTTP','');
insert into tbl_srv_param (USAGE_KEY, SRV_ID, PARAM_USAGE, PARAM_INDEX, PARAM_DATA, PARAM_DSP) values (0,'2810','1','01','LOG_MODE=5','');
insert into tbl_srv_param (USAGE_KEY, SRV_ID, PARAM_USAGE, PARAM_INDEX, PARAM_DATA, PARAM_DSP) values (0,'2810','1','02','LOG_FILE_PATH=$HOME/log/','');
insert into tbl_srv_param (USAGE_KEY, SRV_ID, PARAM_USAGE, PARAM_INDEX, PARAM_DATA, PARAM_DSP) values (0,'2810','1','03','LOG_SWITCH_MODE=1','');
insert into tbl_srv_param (USAGE_KEY, SRV_ID, PARAM_USAGE, PARAM_INDEX, PARAM_DATA, PARAM_DSP) values (0,'2810','1','04','LOG_SIZE=20','');
insert into tbl_srv_param (USAGE_KEY, SRV_ID, PARAM_USAGE, PARAM_INDEX, PARAM_DATA, PARAM_DSP) values (0,'2810','1','05','MSG_COMPRESS_FLAG=N','');
insert into tbl_srv_param (USAGE_KEY, SRV_ID, PARAM_USAGE, PARAM_INDEX, PARAM_DATA, PARAM_DSP) values (0,'2810','1','06','MSQ_NUM_MAX=1000','');
insert into tbl_srv_param (USAGE_KEY, SRV_ID, PARAM_USAGE, PARAM_INDEX, PARAM_DATA, PARAM_DSP) values (0,'2810','1','07','SRV_USAGE_KEY=0','');
insert into tbl_srv_param (USAGE_KEY, SRV_ID, PARAM_USAGE, PARAM_INDEX, PARAM_DATA, PARAM_DSP) values (0,'2810','2','01','1106','');
insert into tbl_srv_param (USAGE_KEY, SRV_ID, PARAM_USAGE, PARAM_INDEX, PARAM_DATA, PARAM_DSP) values (0,'2810','2','02','28093','');
/*
Navicat MySQL Data Transfer

Source Server         : 192.168.43.12
Source Server Version : 80041
Source Host           : 192.168.43.12:3306
Source Database       : ry

Target Server Type    : MYSQL
Target Server Version : 80041
File Encoding         : 65001

Date: 2025-09-03 18:48:50
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for tbl_msq_inf
-- ----------------------------
DROP TABLE IF EXISTS `tbl_msq_inf`;
CREATE TABLE `tbl_msq_inf` (
  `MSQ_INT_ID` char(4) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin NOT NULL,
  `MSQ_KEY` char(16) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin NOT NULL,
  `MSQ_TYPE` char(8) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin NOT NULL,
  UNIQUE KEY `PK_tbl_msq_inf` (`MSQ_INT_ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_bin COMMENT='msq info';
--select * from tbl_msq_inf;
delete from tbl_msq_inf;
insert into tbl_msq_inf (MSQ_INT_ID, MSQ_KEY, MSQ_TYPE) values ('1106','0000000020001106','00001106');
insert into tbl_msq_inf (MSQ_INT_ID, MSQ_KEY, MSQ_TYPE) values ('2810','0000000020002810','00002810');
