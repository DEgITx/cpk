const { createClient } = require('redis');

module.exports = async () => {
    const client = createClient();
    client.on('error', (err) => logTE('redis', 'Redis Client Error', err));
    await client.connect();
    logT('redis', 'connected to redis');

    const get = async (key, ignore = false) => {
        const value = await client.get(key);
        if(!value)
            return;
        if(!ignore)
            logT('redis', 'get', key, '=', value);
        return ((
            (value.charAt(0) == '{' && value.charAt(value.length - 1) == '}') || 
            (value.charAt(0) == '[' && value.charAt(value.length - 1) == ']')
        ) ? JSON.parse(value) : value);
    }

    const set = async (key, value, ignore = false) => {
        if (typeof value == 'object')
            value = JSON.stringify(value);
        if(!ignore)
            logT('redis', 'set', key, '=', value);
        return await client.set(key, value);
    }

    const values = async (filter, handler = null) => {
        const keys = await client.keys(filter);
        logT('redis', 'return values list for filter', filter, 'size', keys.length);
        return Promise.all(keys.map(key => handler ? handler(key, true, true) : get(key, true)))
    }

    const ChangeDBListener = (id, needGetter = false, join = false, needRecursive = false) => ({
        set: async (target, key, value) => {
            target[key] = value;
            // if (needRecursive && !(value instanceof Proxy)) {
            //     target[key] = new Proxy(value, ChangeDBListener(join ? `${id}:${key}` : id, false));
            // }
            if(key != 'length')
                await set(join ? `${id}:${key}` : id, target);
            return true;
        },
        deleteProperty: async (target, key) => {
            if (typeof target[key] != 'undefined') {
                delete target[key];
                await set(join ? `${id}:${key}` : id, target);
            }
            return true;
        },
        get: needGetter ? (async (target, prop) => {
            if (!target[prop] && prop != 'then') {
                target[prop] = await get(join ? `${id}:${prop}` : id);
                if (needRecursive && target[prop]) {
                     target[prop] = new Proxy(target[prop], ChangeDBListener(join ? `${id}:${prop}` : id, false));
                }
            }
            return target[prop];
        }) : undefined,
    })

    const DBobj = {};
    const DB = new Proxy(DBobj, {
        get: async (target, prop) => {
            if (!target[prop]) {
                if(prop == 'packages' || prop == 'archive') {
                    target[prop] = new Proxy({}, ChangeDBListener(`cpk:${prop}`, true, true, true))
                    return target[prop];
                }
                target[prop] = await get(`cpk:${prop}`);
                if (Array.isArray(target[prop])) {
                    target[prop] = new Proxy(target[prop], ChangeDBListener(`cpk:${prop}`));
                }
            }
            return target[prop];
        },
        set: async (target, key, value) => {
            target[key] = value;
            await set(`cpk:${key}`, value);
            return true;
        },
        deleteProperty: async (target, key) => {
            if (typeof target[key] != 'undefined') {
                logT('redis', `del cpk:${key}`);
                await client.del(`cpk:${key}`);
                delete target[key];
            }
            return true;
        },
    });

    return {
        del: async (key) => await client.del(key),
        set,
        get, 
        values,
        DB,
    }
}
