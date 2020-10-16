(*!------------------------------------------------------------
 * [[APP_NAME]] ([[APP_URL]])
 *
 * @link      [[APP_REPOSITORY_URL]]
 * @copyright Copyright (c) [[COPYRIGHT_YEAR]] [[COPYRIGHT_HOLDER]]
 * @license   [[LICENSE_URL]] ([[LICENSE]])
 *------------------------------------------------------------- *)

{---put your main dependencies registration here---}

container.add('db', TMySqlDbFactory.create(
    (container['config'] as IAppConfiguration).getString('db.mysql.version'),
    (container['config'] as IAppConfiguration).getString('db.mysql.host'),
    (container['config'] as IAppConfiguration).getString('db.mysql.database'),
    (container['config'] as IAppConfiguration).getString('db.mysql.username'),
    (container['config'] as IAppConfiguration).getString('db.mysql.password'),
    (container['config'] as IAppConfiguration).getInt('db.mysql.port')
));

container.add(
    'passwHash',
    TArgon2iPasswordHashFactory.create()
        .cost(2)
        .memory(16)
        .paralleism(1)
        .len(16)
);

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

container.add(
    'jwtVerifier',
    TJwtTokenVerifierFactory.create()
        .secret((container['config'] as IAppConfiguration).getString('secretKey'))
        .issuer((container['config'] as IAppConfiguration).getString('jwt.issuer'))
);


container.add(
    'jwtGenerator',
    TJwtTokenGeneratorFactory.create()
        .secret((container['config'] as IAppConfiguration).getString('secretKey'))
        .algorithm(THmacSha256JwtAlg.create())
);
