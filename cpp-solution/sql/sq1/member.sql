-- 成员数据库
CREATE DATABASE IF NOT EXISTS member_db DEFAULT CHARACTER SET utf8mb4;
USE member_db;

DROP TABLE IF EXISTS member;
CREATE TABLE member (
  id INT PRIMARY KEY AUTO_INCREMENT COMMENT '主键ID',
  name VARCHAR(50) NOT NULL COMMENT '姓名',
  account VARCHAR(50) NOT NULL UNIQUE COMMENT '账号',
  phone VARCHAR(20) COMMENT '手机号',
  role VARCHAR(30) COMMENT '角色'
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='成员表';

-- 测试数据
INSERT INTO member(name, account, phone, role)
VALUES ('张三','zhangsan','13800138000','普通成员'),
       ('李四','lisi','13900139000','管理员');