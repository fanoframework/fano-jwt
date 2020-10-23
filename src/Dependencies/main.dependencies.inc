(*!------------------------------------------------------------
 * [[APP_NAME]] ([[APP_URL]])
 *
 * @link      [[APP_REPOSITORY_URL]]
 * @copyright Copyright (c) [[COPYRIGHT_YEAR]] [[COPYRIGHT_HOLDER]]
 * @license   [[LICENSE_URL]] ([[LICENSE]])
 *------------------------------------------------------------- *)

{---put your main dependencies registration here---}

//setup MySQL database utility
container.add('db', TMySqlDbFactory.create(
    config.getString('db.mysql.version'),
    config.getString('db.mysql.host'),
    config.getString('db.mysql.database'),
    config.getString('db.mysql.username'),
    config.getString('db.mysql.password'),
    config.getInt('db.mysql.port')
));

//setup Argon2i password hash verification
container.add(
    'passwHash',
    TArgon2iPasswordHashFactory.create()
        .cost(config.getInt('passwHash.argon2i.iteration'))
        .memory(config.getInt('passwHash.argon2i.memory'))
        .paralleism(config.getInt('passwHash.argon2i.parallel'))
        .len(config.getInt('passwHash.argon2i.hashLen'))
);

//setup authenticator which load user name and password hash from MySQL
container.add(
    'auth',
    TDbPasswHashAuthFactory.create()
        .passwHash(container['passwHash'] as IPasswordHash)
        .rdbms(container['db'] as IRdbms)
        .table('users')
        .credentialColumn('email')
        .passwHashColumn('passw')
        .passwSaltColumn('salt')
);

//setup JWT token verifier
container.add(
    'jwtVerifier',
    TJwtTokenVerifierFactory.create()
        .secret(config.getString('secretKey'))
        .issuer(config.getString('jwt.issuer'))
);


//setup JWT token generation and signing with HMAC SHA256
container.add(
    'jwtGenerator',
    TJwtTokenGeneratorFactory.create()
        .secret(config.getString('secretKey'))
        .algorithm(THmacSha256JwtAlg.create())
);
