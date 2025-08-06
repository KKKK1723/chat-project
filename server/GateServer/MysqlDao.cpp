#include "MysqlDao.h"

MysqlDao::MysqlDao()
{
    auto &cfg = ConfigMgr::Inst();
    const auto &host = cfg["Mysql"]["Host"];
    const auto &port = cfg["Mysql"]["Port"];
    const auto &pwd = cfg["Mysql"]["PassWord"];
    const auto &schema = cfg["Mysql"]["Schema"];
    const auto &user = cfg["Mysql"]["User"];
    std::string url = "tcp://" + host + ":" + port;
    _mysqlpool.reset(new MysqlConPool(url, user, pwd, schema, 5));
}

MysqlDao::~MysqlDao()
{
    _mysqlpool->Close();
}

int MysqlDao::RegisterUser(const std::string &name, const std::string &email, const std::string &password)
{
    std::unique_ptr<SqlConnection> connection = _mysqlpool->GetConnection();
    try
    {
        if (connection == nullptr)
        {
            std::cout << "RegisterUserȡ��һ��������" << std::endl;
            return 0;
        }

        std::unique_ptr<sql::PreparedStatement> prestmt(connection->_connection->prepareStatement("CALL reg_user(?,?,?,@result)"));
        prestmt->setString(1, name);
        prestmt->setString(2, email);
        prestmt->setString(3, password);

        prestmt->execute();

        std::unique_ptr<sql::Statement> resultstmt(connection->_connection->createStatement());
        std::unique_ptr<sql::ResultSet> res(resultstmt->executeQuery("SELECT @result AS result"));

        if (res->next())
        {
            _mysqlpool->ReturnConnection(std::move(connection));
            int result = res->getInt("result");
            std::cout << "Result: " << result << std::endl;
            return result;
        }
        else
        {
            _mysqlpool->ReturnConnection(std::move(connection));
            return -1;
        }
    }
    catch (sql::SQLException &e)
    {
        _mysqlpool->ReturnConnection(std::move(connection));
        std::cerr << "SQLException: " << e.what();
        std::cerr << " (MySQL error code: " << e.getErrorCode();
        std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
        return -1;
    }
}