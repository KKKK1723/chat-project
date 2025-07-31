const config_module = require('./config')
const Redis = require("ioredis")


const RedisClient = new Redis({
    host: config_module.redis_host,
    port: config_module.redis_port,
    password: config_module.redis_passwd,
})


//监听
RedisClient.on("error", function (err) {
    console.log("RedisClient connect error");
    RedisClient.quit();
});


//获取value
async function GetRedisValue(key)
{
    try
    {
        let value = await RedisClient.get(key);
        if (value == null)
        {
            console.log('GetRedisValue value == null');
            return null
        }

        console.log('GetRedisValue value ','<' + v + '>', ' GetRedisValue success' );
        return value;
    }
    catch (error)
    {
        console.log('GetRedis error, error is', error);
        return null
    }
}


//设置key
async function SetRedisKey(key, value, time)
{
    try
    {
        await RedisClient.set(key, value,'EX', time);
        return true;
    }
    catch(error)
    {
        console.log('SetRedisKey error, error is', error);
        return false;
    }
}


//查询key是否存在
async function QueryRedisKey(key)
{
    try
    {
        const result = await RedisCli.exists(key)
        if (result === 0) {
          console.log('result:<','<'+result+'>','This key is null...');
          return null
        }
        console.log('Result:','<'+result+'>','With this value!...');
        return result
    }
    catch (error)
    {
        console.log('QueryRedisKey error, error is', error);
    }
}


//退出
function Quit()
{
    RedisClient.quit();
}


module.exports = { GetRedisValue, SetRedisKey, QueryRedisKey, Quit };