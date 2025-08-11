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

bool MysqlDao::CheckEmail(const std::string &name, const std::string &email)
{

    auto con = _mysqlpool->GetConnection();
    if (con == nullptr)
    {
        std::cout << "从mysqlpool中获取空连接" << std::endl;
        return false;
    }

    try
    {
        std::unique_ptr<sql::PreparedStatement> pstmt(con->_connection->prepareStatement("SELECT email FROM user WHERE name = ?"));

        pstmt->setString(1, name);

        std::unique_ptr<sql::ResultSet> rest(pstmt->executeQuery());

        while (rest->next())
        {
            std::cout << "Check Email: " << rest->getString("email") << std::endl;
            if (email != rest->getString("email"))
            {
                _mysqlpool->ReturnConnection(std::move(con));
                return false;
            }
            _mysqlpool->ReturnConnection(std::move(con));
            return true;
        }
    }
    catch (sql::SQLException &e)
    {
        _mysqlpool->ReturnConnection(std::move(con));
        std::cerr << "SQLException: " << e.what();
        std::cerr << " (MySQL error code: " << e.getErrorCode();
        std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
        return false;
    }
}

bool MysqlDao::UpdatePwd(const std::string &name, const std::string &pwd)
{
    auto con = _mysqlpool->GetConnection();
    if (con == nullptr)
    {
        std::cout << "从mysqlpool中获取空连接" << std::endl;
        return false;
    }

    try
    {
        std::unique_ptr<sql::PreparedStatement> pstmt(con->_connection->prepareStatement("UPDATE user SET pwd = ? WHERE name = ?"));
        pstmt->setString(1, pwd);
        pstmt->setString(2, name);

        int ans = pstmt->executeUpdate();

        std::cout << "Updated rows: " << ans << std::endl;
        _mysqlpool->ReturnConnection(std::move(con));
        return true;
    }
    catch (sql::SQLException &e)
    {
        _mysqlpool->ReturnConnection(std::move(con));
        std::cerr << "SQLException: " << e.what();
        std::cerr << " (MySQL error code: " << e.getErrorCode();
        std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
        return false;
    }
}

bool MysqlDao::CheckPwd(const std::string &name, const std::string &pwd, UserInfo &userinfo)
{
    auto con = _mysqlpool->GetConnection();
    if (con == nullptr)
    {
        std::cout << "从mysqlpool中获取空连接" << std::endl;
        return false;
    }

    Defer defer([this, &con]()
                { _mysqlpool->ReturnConnection(std::move(con)); });

    try
    {
        std::unique_ptr<sql::PreparedStatement> pstmt(con->_connection->prepareStatement("SELECT * FROM user WHERE name = ?"));
        pstmt->setString(1, name);

        std::unique_ptr<sql::ResultSet> rest(pstmt->executeQuery());

        std::string pwdd = "";
        if (rest->next())
        {
            pwdd = rest->getString("pwd");
            std::cout << "Password: " << pwdd << std::endl;
        }
        else
        {
            return false;
        }

        userinfo.name = name;
        userinfo.pwd = pwdd;
        userinfo.email = rest->getString("email");
        userinfo.uid = rest->getInt("uid");
        return true;
    }
    catch (sql::SQLException &e)
    {
        std::cerr << "SQLException: " << e.what();
        std::cerr << " (MySQL error code: " << e.getErrorCode();
        std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
        return false;
    }
}