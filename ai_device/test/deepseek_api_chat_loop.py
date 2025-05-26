
import os
import time
from datetime import datetime
from openai import OpenAI
import pytz

# # 读取.env文件，加载环境变量
# load_dotenv()
DS_API_KEY = "sk-084b49c364754aa2bc626edf2cf20fd6"

class DeepSeekChat:
    """DeepSeek对话管理类"""
  
    def __init__(self):
        """初始化对话系统"""
        # 初始化时间统计（放在最前面防止属性缺失）
        self.start_time = time.time()  # 记录程序启动时间
      
        try:
            # 初始化OpenAI客户端
            self.client = OpenAI(
                api_key=DS_API_KEY,  # 从.env文件读取密钥
                base_url="https://api.deepseek.com"     # DeepSeek API地址
            )
        except Exception as e:
            print(f"初始化失败: {str(e)}")
            raise

        # 初始化对话记录
        self.messages = [
            {"role": "system", "content": "你是我的人工智能助手"}
        ]

        self.response = self.client.chat.completions.create(
                    model="deepseek-chat",
                    messages=self.messages,
                    stream=False
                )
        self.reply = self.response.choices[0].message.content
        self.usage = self.response.usage  # token使用情况
        # 打印一次初始化情况
        print(f"助手: {self.reply}")

        # 初始化统计指标
        self.total_prompt_tokens = 0     # 累计输入token
        self.total_completion_tokens = 0 # 累计输出token
        self.total_cost = 0.0            # 累计总费用

    def _get_beijing_time(self) -> datetime:
        """获取当前北京时间（精确到分钟）
      
        返回:
            datetime: 北京时区的时间对象
        """
        utc_now = datetime.utcnow().replace(tzinfo=pytz.utc)
        return utc_now.astimezone(pytz.timezone('Asia/Shanghai'))        

    def _calculate_cost(self, prompt_tokens: int, completion_tokens: int) -> float:
        """计算费用（根据DeepSeek的时段定价）
      
        参数:
            prompt_tokens: 输入token数量
            completion_tokens: 输出token数量
          
        返回:
            float: 计算出的费用金额
        """
        beijing_time = self._get_beijing_time()
        current_minute = beijing_time.hour * 60 + beijing_time.minute
      
        # 判断计费时段（00:30-08:30为优惠时段）
        if 30 <= current_minute < 510:  # 00:30(30) 到 08:30(510)
            input_rate = 1 / 1_000_000   # 优惠时段输入费率
            output_rate = 4 / 1_000_000  # 优惠时段输出费率
        else:
            input_rate = 4 / 1_000_000   # 常规时段输入费率
            output_rate = 16 / 1_000_000 # 常规时段输出费率
          
        return (prompt_tokens * input_rate) + (completion_tokens * output_rate)

    def _show_stats(self, usage: dict, cost: float):
        """显示统计信息
      
        参数:
            usage: API返回的token使用情况
            cost: 本次对话费用
        """
        # 更新累计数据
        self.total_prompt_tokens += usage.prompt_tokens
        self.total_completion_tokens += usage.completion_tokens
        self.total_cost += cost
      
        # 打印本次对话统计
        # print(f"\n【本次统计】")
        # print(f"输入token: {usage.prompt_tokens} | 输出token: {usage.completion_tokens}")
        # print(f"本次费用: ￥{cost:.6f}")
      
        # 打印累计统计
        print(f"\n【累计统计】")
        print(f"总输入token: {self.total_prompt_tokens}")
        print(f"总输出token: {self.total_completion_tokens}")
        print(f"总费用: ￥{self.total_cost:.6f}\n")

    def chat_loop(self):
        """启动对话循环"""
        print("DeepSeek对话系统已启动（输入 'exit' 退出）")
        while True:
            try:
                # 获取用户输入
                user_input = input("\n用户: ").strip()
                if not user_input:
                    continue    # 忽略空输入
                  
                if user_input.lower() == 'exit':
                    break    # 退出命令
                  
                # 添加用户消息到对话历史
                self.messages.append({"role": "user", "content": user_input})
              
                # 发送API请求
                response = self.client.chat.completions.create(
                    model="deepseek-reasoner",
                    messages=self.messages,
                    stream=False
                )
              
                # 处理响应
                self.reply = response.choices[0].message.content
                self.usage = response.usage  # token使用情况
              
                # 计算费用
                # cost = self._calculate_cost(usage.prompt_tokens, usage.completion_tokens)
              
                # 显示统计信息
                # self._show_stats(usage, cost)
              
                # 添加ai助手的回复到对话历史
                self.messages.append({"role": "assistant", "content": self.reply})
                print(f"助手: {self.reply}")

            except KeyboardInterrupt:
                print("\n检测到中断，正在退出...")
                break
            except Exception as e:
                print(f"\n发生错误: {str(e)}")
                continue

        """对象销毁时显示运行时长"""
        if hasattr(self, 'start_time'):
            total_time = time.time() - self.start_time
            hours, remainder = divmod(total_time, 3600)
            minutes, seconds = divmod(remainder, 60)
            print(f"\n总运行时间: {int(hours):02d}:{int(minutes):02d}:{int(seconds):02d}")

            # 计算费用
            cost = self._calculate_cost(self.usage.prompt_tokens, self.usage.completion_tokens)
            
            # 显示统计信息
            self._show_stats(self.usage, cost)

    # def __del__(self):
        

if __name__ == "__main__":
    try:
        chat = DeepSeekChat()
        chat.chat_loop()
    except Exception as e:
        print(f"系统启动失败: {str(e)}")
