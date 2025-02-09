const t = require('tap')
const { fake: mockNpm } = require('../../fixtures/mock-npm')

let result = []

const noop = () => null
const config = {
  'git-tag-version': true,
  'tag-version-prefix': 'v',
  json: false,
}
const npm = mockNpm({
  config,
  prefix: '',
  version: '1.0.0',
  output: (...msg) => {
    for (const m of msg)
      result.push(m)
  },
})
const mocks = {
  libnpmversion: noop,
}

const Version = t.mock('../../../lib/commands/version.js', mocks)
const version = new Version(npm)

const _processVersions = process.versions
t.afterEach(() => {
  config.json = false
  npm.prefix = ''
  process.versions = _processVersions
  result = []
})

t.test('no args', async t => {
  const prefix = t.testdir({
    'package.json': JSON.stringify({
      name: 'test-version-no-args',
      version: '3.2.1',
    }),
  })
  npm.prefix = prefix
  Object.defineProperty(process, 'versions', { value: { node: '1.0.0' } })

  await version.exec([])

  t.same(
    result,
    [{
      'test-version-no-args': '3.2.1',
      node: '1.0.0',
      npm: '1.0.0',
    }],
    'should output expected values for various versions in npm'
  )
})

t.test('too many args', async t => {
  await t.rejects(
    version.exec(['foo', 'bar']),
    /npm version/,
    'should throw usage instructions error'
  )
})

t.test('completion', async t => {
  const testComp = async (argv, expect) => {
    const res = await version.completion({ conf: { argv: { remain: argv } } })
    t.strictSame(res, expect, argv.join(' '))
  }

  await testComp(['npm', 'version'], [
    'major',
    'minor',
    'patch',
    'premajor',
    'preminor',
    'prepatch',
    'prerelease',
    'from-git',
  ])
  await testComp(['npm', 'version', 'major'], [])

  t.end()
})

t.test('failure reading package.json', async t => {
  const prefix = t.testdir({})
  npm.prefix = prefix

  await version.exec([])

  t.same(
    result,
    [{
      npm: '1.0.0',
      node: '1.0.0',
    }],
    'should not have package name on returning object'
  )
})

t.test('--json option', async t => {
  const prefix = t.testdir({})
  config.json = true
  npm.prefix = prefix
  Object.defineProperty(process, 'versions', { value: {} })

  await version.exec([])
  t.same(
    result,
    ['{\n  "npm": "1.0.0"\n}'],
    'should return json stringified result'
  )
})

t.test('with one arg', async t => {
  const Version = t.mock('../../../lib/commands/version.js', {
    ...mocks,
    libnpmversion: (arg, opts) => {
      t.equal(arg, 'major', 'should forward expected value')
      t.same(
        opts,
        {
          path: '',
        },
        'should forward expected options'
      )
      return '4.0.0'
    },
  })
  const version = new Version(npm)

  await version.exec(['major'])
  t.same(result, ['v4.0.0'], 'outputs the new version prefixed by the tagVersionPrefix')
})

t.test('workspaces', async t => {
  t.teardown(() => {
    npm.localPrefix = ''
    npm.prefix = ''
  })

  t.test('no args, all workspaces', async t => {
    const testDir = t.testdir({
      'package.json': JSON.stringify({
        name: 'workspaces-test',
        version: '1.0.0',
        workspaces: ['workspace-a', 'workspace-b'],
      }, null, 2),
      'workspace-a': {
        'package.json': JSON.stringify({
          name: 'workspace-a',
          version: '1.0.0',
        }),
      },
      'workspace-b': {
        'package.json': JSON.stringify({
          name: 'workspace-b',
          version: '1.0.0',
        }),
      },
    })
    npm.localPrefix = testDir
    npm.prefix = testDir
    const version = new Version(npm)
    await version.execWorkspaces([], [])
    t.same(result, [{
      'workspaces-test': '1.0.0',
      'workspace-a': '1.0.0',
      'workspace-b': '1.0.0',
      npm: '1.0.0',
    }], 'outputs includes main package and workspace versions')
  })

  t.test('no args, single workspaces', async t => {
    const testDir = t.testdir({
      'package.json': JSON.stringify({
        name: 'workspaces-test',
        version: '1.0.0',
        workspaces: ['workspace-a', 'workspace-b'],
      }, null, 2),
      'workspace-a': {
        'package.json': JSON.stringify({
          name: 'workspace-a',
          version: '1.0.0',
        }),
      },
      'workspace-b': {
        'package.json': JSON.stringify({
          name: 'workspace-b',
          version: '1.0.0',
        }),
      },
    })
    npm.localPrefix = testDir
    npm.prefix = testDir
    const version = new Version(npm)
    await version.execWorkspaces([], ['workspace-a'])
    t.same(result, [{
      'workspaces-test': '1.0.0',
      'workspace-a': '1.0.0',
      npm: '1.0.0',
    }], 'outputs includes main package and requested workspace versions')
  })

  t.test('no args, all workspaces, workspace with missing name or version', async t => {
    const testDir = t.testdir({
      'package.json': JSON.stringify({
        name: 'workspaces-test',
        version: '1.0.0',
        workspaces: ['workspace-a', 'workspace-b', 'workspace-c'],
      }, null, 2),
      'workspace-a': {
        'package.json': JSON.stringify({
          name: 'workspace-a',
          version: '1.0.0',
        }),
      },
      'workspace-b': {
        'package.json': JSON.stringify({
          name: 'workspace-b',
        }),
      },
      'workspace-c': {
        'package.json': JSON.stringify({
          version: '1.0.0',
        }),
      },
    })
    npm.localPrefix = testDir
    npm.prefix = testDir
    const version = new Version(npm)
    await version.execWorkspaces([], [])
    t.same(result, [{
      'workspaces-test': '1.0.0',
      'workspace-a': '1.0.0',
      npm: '1.0.0',
    }], 'outputs includes main package and valid workspace versions')
  })

  t.test('with one arg, all workspaces', async t => {
    const libNpmVersionArgs = []
    const testDir = t.testdir({
      'package.json': JSON.stringify({
        name: 'workspaces-test',
        version: '1.0.0',
        workspaces: ['workspace-a', 'workspace-b'],
      }, null, 2),
      'workspace-a': {
        'package.json': JSON.stringify({
          name: 'workspace-a',
          version: '1.0.0',
        }),
      },
      'workspace-b': {
        'package.json': JSON.stringify({
          name: 'workspace-b',
          version: '1.0.0',
        }),
      },
    })
    const Version = t.mock('../../../lib/commands/version.js', {
      ...mocks,
      libnpmversion: (arg, opts) => {
        libNpmVersionArgs.push([arg, opts])
        return '2.0.0'
      },
    })
    npm.localPrefix = testDir
    npm.prefix = testDir
    const version = new Version(npm)

    await version.execWorkspaces(['major'], [])
    t.same(result, ['workspace-a', 'v2.0.0', 'workspace-b', 'v2.0.0'], 'outputs the new version for only the workspaces prefixed by the tagVersionPrefix')
  })

  t.test('too many args', async t => {
    await t.rejects(
      version.execWorkspaces(['foo', 'bar'], []),
      /npm version/,
      'should throw usage instructions error'
    )
  })
})
